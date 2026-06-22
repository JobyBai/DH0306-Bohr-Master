#include "sensor_control.h"
#include "hmi_driver.h"
#include "spi.h"
#include "usart.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hmi_control.h"

/* ───── DMA 环形缓冲区 ───── */
static uint8_t dma_buf[SENSOR_DMA_BUF_SIZE];
static uint16_t read_pos = 0; // 上次读取到的位置（读指针）

/* ───── 对外数据 ───── */
float period = 0.0f;      // 周期
float amplitude = 0.0f;   // 振幅
float phase_diff1 = 0.0f; // 相位差1
float phase_diff2 = 0.0f; // 相位差2
uint16_t index_mode1 = 0;
uint16_t index_mode2 = 0;
uint16_t index_mode3 = 0;
uint8_t sensor_frame_len = 10; // 单帧总字节数
uint8_t page_flag = 1;//当前页面标志位
uint8_t measure_flag = 0; // 测量标志位 // 0:停止测量，1：自由振荡测量，2：阻尼振荡测量，3：受迫振荡测量
Record_t free_record_buf[200];   // 自由振荡测量记录缓存
Record_t damped_record_buf[200]; // 阻尼振荡测量记录缓存
Record_t forced_record_buf[200]; // 受迫振荡测量记录缓存
Record_t filtered_record_buf[200]; // 受迫振荡 筛选结果缓存

uint8_t output_str[128]; // 输出字符串缓存

void Sensor_Control_Init(void) {
  Sensor_Control_Stop();
  read_pos = 0;
  HAL_SPI_Receive_DMA(&hspi2, dma_buf, SENSOR_DMA_BUF_SIZE);
}

// 停止模式
void Sensor_Control_Stop(void) {
  A2_LOW();
  A1_LOW();
  HAL_Delay(10);
}

// 启动模式1
void Sensor_Control_Start_Mode1(void) {
  sensor_frame_len = 10;
  Sensor_Control_Stop();
  A2_LOW();
  A1_HIGH();
}

// 启动模式2
void Sensor_Control_Start_Mode2(void) {
  sensor_frame_len = 18;
  Sensor_Control_Stop();
  A2_HIGH();
  A1_LOW();
}

/* ───── 私有：获取 DMA 当前写入位置 ───── */
/**
 * @brief 通过 DMA 剩余计数反推当前写到了哪个位置
 *        写指针 = 总长度 - DMA剩余未传数量
 */
static inline uint16_t get_dma_write_pos(void) {
  return (uint16_t)(SENSOR_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(hspi2.hdmarx));
}

/* ───── 私有：从环形缓冲区取一帧 ───── */
/**
 * @brief 在 read_pos 到 write_pos 之间搜索完整帧
 *        找到帧头后校验剩余长度是否足够，足够则解析并移动读指针
 * @return true  成功解析一帧
 *         false 没有完整帧
 */
static bool fetch_frame(uint8_t *frame_out) {
  uint16_t write_pos = get_dma_write_pos();

  /* 计算环形缓冲区中待处理字节数 */
  uint16_t available =
      (write_pos - read_pos + SENSOR_DMA_BUF_SIZE) % SENSOR_DMA_BUF_SIZE;

  if (available < sensor_frame_len)
    return false; // 数据不足一帧，等下次

  /* 在待处理区域内搜索帧头 */
  for (uint16_t i = 0; i < available; i++) {
    uint16_t idx = (read_pos + i) % SENSOR_DMA_BUF_SIZE;

    if (dma_buf[idx] != SENSOR_FRAME_HEADER)
      continue;

    /* 找到帧头，检查后续字节是否够一整帧 */
    if (available - i < sensor_frame_len)
      break; // 帧头之后数据不足，等下次 DMA 继续填充

    /* 拷贝完整帧到线性缓冲区（处理环形回绕） */
    for (uint16_t j = 0; j < sensor_frame_len; j++)
      frame_out[j] = dma_buf[(idx + j) % SENSOR_DMA_BUF_SIZE];

    /* 移动读指针到帧尾之后 */
    read_pos = (idx + sensor_frame_len) % SENSOR_DMA_BUF_SIZE;
    return true;
  }

  /* 没找到有效帧头，跳过这段脏数据，读指针追上写指针 */
  read_pos = write_pos;
  return false;
}

/* ───── 私有：解析并记录一帧 ───── */
static void process_frame(const uint8_t *frame) {
  memcpy(&period, &frame[2], sizeof(float));
  memcpy(&amplitude, &frame[6], sizeof(float));

  if (frame[1] == SENSOR_MODE1_ID) {
    // 自由振荡测量
    if (page_flag == 1) {
      // 实时展示
      SetTextFloat(0x01, 0x07, period, 4, 1);
      SetTextFloat(0x01, 0x06, amplitude, 1, 1);

      // 表格展示，并保存
      if (measure_flag == 1) {
        if (index_mode1 >= 200) {
          // 停止采集 or 标记完成
          measure_flag = 0;
          return;
        }

        snprintf((char *)output_str, sizeof(output_str), "%u;%.1f;%.4f",
                 index_mode1, amplitude, period);
        Record_Add(1, 1, output_str);
        free_record_buf[index_mode1].index = index_mode1;
        free_record_buf[index_mode1].amplitude = amplitude;
        free_record_buf[index_mode1].period = period;
        free_record_buf[index_mode1].drive_period = 0.0f;
        free_record_buf[index_mode1].damping = 0; // 无阻尼振荡
        free_record_buf[index_mode1].phase_diff1 = 0.0f;
        free_record_buf[index_mode1].phase_diff2 = 0.0f;
        biaoge_bottom(1,1,index_mode1);// 展示对应行数
        index_mode1++;
      }
    } else if (page_flag == 2) {

      // 实时展示
      SetTextFloat(0x02, 0x09, period, 4, 1);
      SetTextFloat(0x02, 0x0A, amplitude, 1, 1);
      // 表格展示，并保存
      if (measure_flag == 2) {
        if (index_mode2 >= 200) {
          // 停止采集 or 标记完成
          measure_flag = 0;
          return;
        }

        snprintf((char *)output_str, sizeof(output_str), "%u;%.1f;%.4f",
                 index_mode2, amplitude, period);
        Record_Add(2, 1, output_str);
        damped_record_buf[index_mode2].index = index_mode2;
        damped_record_buf[index_mode2].amplitude = amplitude;
        damped_record_buf[index_mode2].period = period;
        damped_record_buf[index_mode2].drive_period = 0.0f;
        damped_record_buf[index_mode2].damping = damping_num; // 阻尼档位
        damped_record_buf[index_mode2].phase_diff1 = 0.0f;
        damped_record_buf[index_mode2].phase_diff2 = 0.0f;
        biaoge_bottom(2,1,index_mode2);// 展示对应行数
        index_mode2++;
      }
    }else if (page_flag == 3) {// 当电机不转动的时候，也会显示周期和幅值
      SetTextFloat(0x03, 0x0C, period, 4, 1);
      SetTextFloat(0x03, 0x0D, amplitude, 1, 1);
    }

  } else if (frame[1] == SENSOR_MODE2_ID) {
    SetTextFloat(0x03, 0x0C, period, 4, 1);
    SetTextFloat(0x03, 0x0D, amplitude, 1, 1);
    memcpy(&phase_diff1, &frame[10], sizeof(float));
    memcpy(&phase_diff2, &frame[14], sizeof(float));
    SetTextFloat(0x03, 0x0E, phase_diff1, 1, 1);
    SetTextFloat(0x03, 0x0F, phase_diff2, 1, 1);

    //原来保存到表格
    // SetTextFloat(0x03, 0x0C, period, 3, 1);
    // SetTextFloat(0x03, 0x0D, amplitude, 3, 1);
    // // 表格展示，并保存
    // if (measure_flag == 3) {
    //   if (index_mode3 >= 500) {
    //     // 停止采集 or 标记完成
    //     measure_flag = 0;
    //     return;
    //   }

    //   memcpy(&phase_diff1, &frame[10], sizeof(float));
    //   memcpy(&phase_diff2, &frame[14], sizeof(float));
    //   SetTextFloat(0x03, 0x0E, phase_diff1, 3, 1);
    //   SetTextFloat(0x03, 0x0F, phase_diff2, 3, 1);
    //   snprintf((char *)output_str, sizeof(output_str),
    //            "%u;%.3f;%.3f;%.3f;%.3f", index_mode3, amplitude, period,
    //            phase_diff1, phase_diff2);
    //   Record_Add(3, 1, output_str);
    //   forced_record_buf[index_mode3].index = index_mode3;
    //   forced_record_buf[index_mode3].amplitude = amplitude;
    //   forced_record_buf[index_mode3].period = period;
    //   forced_record_buf[index_mode3].drive_period = 0.0f;
    //   forced_record_buf[index_mode3].damping = 0; // 无阻尼振荡
    //   forced_record_buf[index_mode3].phase_diff1 = phase_diff1;
    //   forced_record_buf[index_mode3].phase_diff2 = phase_diff2;
    //   index_mode3++;
    // }
  }
}

/* ───── 公开函数 ───── */

void Sensor_Init(void) {
  read_pos = 0;
  HAL_SPI_Receive_DMA(&hspi2, dma_buf, SENSOR_DMA_BUF_SIZE);
}

void Sensor_Poll(void) {
  uint8_t frame[sensor_frame_len];

  /* 一次 Poll 尽可能消费所有完整帧 */
  while (fetch_frame(frame)) {
    process_frame(frame);
  }
}

// 传感器数据缓存
// uint8_t sensor_data[20];
// _Bool sensor_receive_flag = false;
// uint8_t output_str[50];  // 输出字符串缓存
// float period;// 周期
// float amplitude;// 振幅

// uint32_t index_mode1 = 0 ;
// uint32_t index_mode2 = 0 ;

// void process_sensor_data(uint8_t index)
// {
//     memcpy(&period,    &sensor_data[2], sizeof(float));
//     memcpy(&amplitude, &sensor_data[6], sizeof(float));
//     index_mode1++;
//     // 拼接字符串： "序号；周期；振幅"
//     snprintf(output_str, sizeof(output_str), "%d;%.3f;%.3f", index_mode1,
//     period, amplitude); Record_Add(1, 1, output_str);
// }

// // 接收SPI2信息
// void Sensor_Receive(void)
// {
//     // 接收SPI2信息
//     HAL_SPI_Receive(&hspi2, sensor_data, 10, 100);
//     // 模式一数据处理
//     if(sensor_data[0] == 0xEE && sensor_data[1] ==0xBE){
//         process_sensor_data(1);
//         // 清空数组
// //        memset(sensor_data, 0, sizeof(sensor_data));
//     }
//     // 模式二数据处理
//     else if(sensor_data[0] == 0xEE && sensor_data[1] ==0xBB){
//        process_sensor_data(2);
//        Record_Add(2,1,output_str);
//     }

//     sensor_receive_flag = true;
// }
