#
# Copyright (C) 2020 LineageOS
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Inherit from grus device.
$(call inherit-product, $(LOCAL_PATH)/device.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_DEVICE := grus
PRODUCT_NAME := lineage_grus
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := MI 9 SE
PRODUCT_MANUFACTURER := Xiaomi

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="grus-user 9 PKQ1.181121.001 V11.0.4.0.PFBMIXM release-keys"

BUILD_FINGERPRINT := "Xiaomi/grus/grus:9/PKQ1.181121.001/V11.0.4.0.PFBMIXM:user/release-keys"

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME="grus" \
    TARGET_DEVICE="grus"