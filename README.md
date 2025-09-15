<img width="808" height="597" alt="스크린샷 2025-09-14 171332" src="https://github.com/user-attachments/assets/4c21613a-dd9d-4cde-a5e3-dd1ea738db5d" /># STM32F765 기반 커스텀 드론 비행 컨트롤러(FC) 개발

**현장 경험을 바탕으로 안정성과 신뢰성을 확보한 고성능 드론 FC를 직접 설계하고 개발하는 개인 프로젝트입니다.**

## 📖 프로젝트 소개
기성품 FC의 한계를 넘어, 특정 임무(정밀 비행, 안정적인 촬영 등)에 최적화된 드론을 개발하기 위해 시스템 아키텍처 설계부터 하드웨어 회로 설계, 펌웨어 개발까지 전 과정을 직접 수행합니다.

## ✨ 주요 기능
- [ ] IMU 센서(MPU-6050) 데이터 수신 및 처리
- [ ] Barometer 센서(BMP280)를 이용한 고도 추정
- [ ] PWM 신호를 이용한 BLDC 모터 제어
- [ ] (앞으로 개발할 기능들을 여기에 적어보세요)

## 🛠️ 기술 스택
- **MCU:** `STM32F765VI (ARM Cortex-M7)`
- **Language:** `C`, `C++`
- **Hardware Design:** `KiCad`
- **IDE:** `STM32CubeIDE`
- **Library:** `STM32 HAL Driver`

## 🏗️ 시스템 아키텍처
<img width="808" height="597" alt="스크린샷 2025-09-14 171332" src="https://github.com/user-attachments/assets/ead54a0e-4ece-4034-9dd7-6b89ea3470b8" />

## 🔌 하드웨어 회로도
<img width="1356" height="873" alt="스크린샷 2025-09-14 170841" src="https://github.com/user-attachments/assets/cbdbd0f7-f369-41e2-8228-cc41f8339667" />


## 🚀 앞으로의 개발 계획
- [ ] FreeRTOS를 적용하여 멀티태스킹 환경 구축
- [ ] GPS 모듈을 연동하여 위치 기반 자율 비행 기능 구현
- [ ] PID 제어 알고리즘 튜닝을 통한 비행 안정성 향상
