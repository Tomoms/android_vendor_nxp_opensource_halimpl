/******************************************************************************
 *
 *  Copyright 2018 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#define LOG_TAG "android.hardware.nfc@1.0-impl"
#include <log/log.h>
#include "Nfc.h"
#include "phNfcStatus.h"

extern "C" {
#include "phNxpNciHal_Adaptation.h"
}

#define CHK_STATUS(x) ((x) == NFCSTATUS_SUCCESS) \
      ? (V1_0::NfcStatus::OK) : (V1_0::NfcStatus::FAILED)


namespace android {
namespace hardware {
namespace nfc {
namespace V1_0 {
namespace implementation {

sp<V1_0::INfcClientCallback> Nfc::mCallbackV1_0 = nullptr;

// Methods from ::android::hardware::nfc::V1_0::INfc follow.
Return<V1_0::NfcStatus> Nfc::open(
    const sp<V1_0::INfcClientCallback>& clientCallback) {
  if (clientCallback == nullptr) {
    ALOGE("Nfc::open null callback");
    return V1_0::NfcStatus::FAILED;
  } else {
    mCallbackV1_0 = clientCallback;
    mCallbackV1_0->linkToDeath(this, 0 /*cookie*/);
  }

  NFCSTATUS status = phNxpNciHal_open(eventCallback, dataCallback);
  return CHK_STATUS(status);
}

Return<uint32_t> Nfc::write(const hidl_vec<uint8_t>& data) {
  hidl_vec<uint8_t> copy = data;
  return phNxpNciHal_write(copy.size(), &copy[0]);
}

Return<V1_0::NfcStatus> Nfc::coreInitialized(const hidl_vec<uint8_t>& data) {
  hidl_vec<uint8_t> copy = data;
  NFCSTATUS status = phNxpNciHal_core_initialized(&copy[0]);
  return CHK_STATUS(status);
}

Return<V1_0::NfcStatus> Nfc::prediscover() {
  NFCSTATUS status = phNxpNciHal_pre_discover();
  return CHK_STATUS(status);
}

Return<V1_0::NfcStatus> Nfc::close() {
  if (mCallbackV1_0 == nullptr) {
    return V1_0::NfcStatus::FAILED;
  }
  NFCSTATUS status = phNxpNciHal_close();

  if (mCallbackV1_0 != nullptr) {
    mCallbackV1_0->unlinkToDeath(this);
    mCallbackV1_0 = nullptr;
  }
  return CHK_STATUS(status);
}

Return<V1_0::NfcStatus> Nfc::controlGranted() {
  NFCSTATUS status = phNxpNciHal_control_granted();
  return CHK_STATUS(status);
}

Return<V1_0::NfcStatus> Nfc::powerCycle() {
  NFCSTATUS status = phNxpNciHal_power_cycle();
  return CHK_STATUS(status);
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace nfc
}  // namespace hardware
}  // namespace android
