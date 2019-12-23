/*
 * Copyright (C) 2019 The LineageOS Project
 * QPNP haptic additions by faust93
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "VibratorService"

#include <log/log.h>

#include <hardware/hardware.h>
#include <hardware/vibrator.h>

#include "Vibrator.h"

#include <cinttypes>
#include <cmath>
#include <iostream>
#include <fstream>

namespace {

using Status = ::android::hardware::vibrator::V1_0::Status;
using EffectStrength = ::android::hardware::vibrator::V1_0::EffectStrength;

static uint8_t convertEffectStrength(EffectStrength strength) {
    uint8_t amplitude = 1;

    switch (strength) {
    case EffectStrength::LIGHT:
        amplitude = 66;
        break;
    case EffectStrength::MEDIUM:
        amplitude = 130;
        break;
    default:
    case EffectStrength::STRONG:
        amplitude = 255;
        break;
    }

    return amplitude;
}

}  // anonymous namespace

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_1 {
namespace implementation {

static constexpr int MAX_INTENSITY = 2250;
static constexpr int MIN_INTENSITY = 1;

static constexpr uint32_t CLICK_TIMING_MS = 10;
static constexpr uint32_t TICK_TIMING_MS = 1;

using Status = ::android::hardware::vibrator::V1_0::Status;
using EffectStrength = ::android::hardware::vibrator::V1_0::EffectStrength;

Vibrator::Vibrator(std::ofstream&& enable, std::ofstream&& duration, std::ofstream&& amplitude, std::ofstream&& amplitude_call, std::ofstream&& amplitude_notif) :
        mEnable(std::move(enable)),
        mDuration(std::move(duration)),
        mAmplitude(std::move(amplitude)),
        mAmplitudeCall(std::move(amplitude_call)),
        mAmplitudeNotif(std::move(amplitude_notif)) {}

// Methods from ::android::hardware::vibrator::V1_1::IVibrator follow.
Return<Status> Vibrator::on(uint32_t timeout_ms) {
    mDuration << timeout_ms << std::endl;
    if (!mDuration) {
        ALOGE("Failed to turn vibrator on (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    return Status::OK;
}

Return<Status> Vibrator::off()  {
    mEnable << 0 << std::endl;
    if (!mEnable) {
        ALOGE("Failed to turn vibrator off (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    return Status::OK;
}

Return<bool> Vibrator::supportsAmplitudeControl()  {
    return true;
}

Return<Status> Vibrator::setAmplitude(uint8_t amplitude) {
    if (amplitude == 0) {
        return Status::BAD_VALUE;
    }

    // Scale the intensity such that an amplitude of 1 is MIN_INTENSITY, an amplitude of 255 is
    // MAX_INTENSITY, and there are equal steps for every value in between.
    long intensity =  std::lround((amplitude - 1) / 254.0 * (MAX_INTENSITY - MIN_INTENSITY) + MIN_INTENSITY);

    mAmplitude << intensity << std::endl;
    mAmplitudeCall << intensity << std::endl;
    mAmplitudeNotif << intensity << std::endl;
    if (!mAmplitude || !mAmplitudeCall || !mAmplitudeNotif) {
        ALOGE("Failed to set amplitude (%d): %s", errno, strerror(errno));
        return Status::UNKNOWN_ERROR;
    }
    mEnable << 1 << std::endl;

    return Status::OK;
}

Return<void> Vibrator::perform(V1_0::Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    return perform_1_1(static_cast<V1_1::Effect_1_1>(effect), strength, _hidl_cb);
}

Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1 effect, EffectStrength strength, perform_cb _hidl_cb) {
    if (effect == V1_1::Effect_1_1::CLICK) {
        uint8_t amplitude = convertEffectStrength(strength);

        on(CLICK_TIMING_MS);
        setAmplitude(amplitude);
        _hidl_cb(Status::OK, CLICK_TIMING_MS);

        return Void();
    }

    if (effect == V1_1::Effect_1_1::TICK) {
        uint8_t amplitude = convertEffectStrength(strength);

        on(TICK_TIMING_MS);
        setAmplitude(amplitude);
        _hidl_cb(Status::OK, TICK_TIMING_MS);

        return Void();
    }

    _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
    return Void();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
