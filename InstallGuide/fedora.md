Installation instructions for Fedora
=====================================

- Install the latest Arduino IDE from [arduino.cc](https://www.arduino.cc/en/Main/Software). `$ sudo dnf -y install arduino` will most likely install an older release.
- Open Terminal and execute the following command (copy->paste and hit enter):

  ```bash
  sudo usermod -a -G dialout $USER && \
  sudo dnf install git python3-pip python3-pyserial && \
  mkdir -p ~/Arduino/hardware && \
  cd ~/Arduino/hardware && \
  git clone https://github.com/HelTecAutomation/ASR650x-Arduino.git && \
  cd ASR650x-Arduino && \
  git submodule update --init --recursive && \
  cd tools && \
  python get.py
  mv gcc-arm-none-eabi-8-2019-q3-update gcc-arm-none-eabi
  sudo chown root:root gcc-arm-none-eabi/bin/arm-none-eabi-g++
  sudo chmod u+s gcc-arm-none-eabi/bin/arm-none-eabi-g++
  ```
  ```
  
  ```
- Restart Arduino IDE