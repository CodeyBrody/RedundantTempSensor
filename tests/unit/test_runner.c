#include "unity.h"

// void test_findAverageValidTemp_allSensorsValid(void);
// void test_findAverageValidTemp_ignoresFaultySensor0(void);
// void test_findAverageValidTemp_ignoresFaultySensor1(void);
// void test_findAverageValidTemp_ignoresFaultySensor2(void);
// void test_findAverageValidTemp_onlySensor2Valid(void);
// void test_findAverageValidTemp_onlySensor0Valid(void);
// void test_findAverageValidTemp_onlySensor1Valid(void);
// void test_findAverageValidTemp_noValidSensorsReturnsNaN(void);

/*OUTLIER TEST DECLARATIONS*/
void test_outlier_detection_agreeingSensorsRemainValid(void);
void test_outlier_detection_marksSensor0OutlierBelowInvalid(void);
void test_outlier_detection_marksSensor1OutlierBelowInvalid(void);
void test_outlier_detection_marksSensor2OutlierBelowInvalid(void);
void test_outlier_detection_marksSensor0OutlierAboveInvalid(void);
void test_outlier_detection_marksSensor1OutlierAboveInvalid(void);
void test_outlier_detection_marksSensor2OutlierAboveInvalid(void);
void test_outlier_detection_marksAllOutliers(void);

/*SENSOR MISSING TEST DECLARATIONS*/
void test_missing_sensors_sensor0MissingNoOutliers(void);
void test_missing_sensors_sensor0MissingOthersOutliers(void);
void test_missing_sensors_sensor1MissingNoOutliers(void);
void test_missing_sensors_sensor1MissingOthersOutliers(void);
void test_missing_sensors_sensor2MissingNoOutliers(void);
void test_missing_sensors_sensor2MissingOthersOutliers(void);
void test_missing_sensors_sensor0MissingOthersOutliersPermutation2(void);
void test_missing_sensors_sensor1MissingOthersOutliersPermutation2(void);
void test_missing_sensors_sensor2MissingOthersOutliersPermutation2(void);
void test_missing_sensors_onlySensor0remaining(void);
void test_missing_sensors_onlySensor1remaining(void);
void test_missing_sensors_onlySensor2remaining(void);
void test_missing_sensors_allSensorsMissing(void);

/*ABOVE BOUNDS ERROR TEST DECLARATIONS*/
void test_aboveOOB_sensor0AboveOthersAgreeing(void);
void test_aboveOOB_sensor1AboveOthersAgreeing(void);
void test_aboveOOB_sensor2AboveOthersAgreeing(void);
void test_aboveOOB_sensor0AboveOthersDisagreeing(void);
void test_aboveOOB_sensor1AboveOthersDisagreeing(void);
void test_aboveOOB_sensor2AboveOthersDisagreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor0InBoundsAllAgreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_sensor1InBoundsAllAgreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_sensor2InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor2InBoundsAllAgreeing(void);
void test_aboveOOB_sensor2InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_allSensorsAboveAgreeing(void);
void test_aboveOOB_allSensorsAboveDisagreeing(void);
void test_aboveOOB_testUpperBoundary(void);

// void test_sensor_readsNegativeTempCorrectly(void);

void setUp(void) {}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    // RUN_TEST(test_findAverageValidTemp_allSensorsValid);
    // RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor0);
    // RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor1);
    // RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor2);
    // RUN_TEST(test_findAverageValidTemp_onlySensor2Valid);
    // RUN_TEST(test_findAverageValidTemp_onlySensor0Valid);
    // RUN_TEST(test_findAverageValidTemp_onlySensor1Valid);
    // RUN_TEST(test_findAverageValidTemp_noValidSensorsReturnsNaN);

    /*OUTLIER TESTS*/
    RUN_TEST(test_outlier_detection_agreeingSensorsRemainValid);
    RUN_TEST(test_outlier_detection_marksSensor0OutlierBelowInvalid);
    RUN_TEST(test_outlier_detection_marksSensor1OutlierBelowInvalid);
    RUN_TEST(test_outlier_detection_marksSensor2OutlierBelowInvalid);
    RUN_TEST(test_outlier_detection_marksSensor0OutlierAboveInvalid);
    RUN_TEST(test_outlier_detection_marksSensor1OutlierAboveInvalid);
    RUN_TEST(test_outlier_detection_marksSensor2OutlierAboveInvalid);
    RUN_TEST(test_outlier_detection_marksAllOutliers);

    /*MISSING SENSOR TEMP DETERMINATION TESTS*/
    RUN_TEST(test_missing_sensors_sensor0MissingNoOutliers);
    RUN_TEST(test_missing_sensors_sensor0MissingOthersOutliers);
    RUN_TEST(test_missing_sensors_sensor1MissingNoOutliers);
    RUN_TEST(test_missing_sensors_sensor1MissingOthersOutliers);
    RUN_TEST(test_missing_sensors_sensor2MissingNoOutliers);
    RUN_TEST(test_missing_sensors_sensor2MissingOthersOutliers);
    RUN_TEST(test_missing_sensors_sensor0MissingOthersOutliersPermutation2);
    RUN_TEST(test_missing_sensors_sensor1MissingOthersOutliersPermutation2);
    RUN_TEST(test_missing_sensors_sensor2MissingOthersOutliersPermutation2);
    RUN_TEST(test_missing_sensors_onlySensor0remaining);
    RUN_TEST(test_missing_sensors_onlySensor1remaining);
    RUN_TEST(test_missing_sensors_onlySensor2remaining);
    RUN_TEST(test_missing_sensors_allSensorsMissing);

    /*OUT OF BOUNDS TESTS*/
    RUN_TEST(test_aboveOOB_sensor0AboveOthersAgreeing);
    RUN_TEST(test_aboveOOB_sensor1AboveOthersAgreeing);
    RUN_TEST(test_aboveOOB_sensor2AboveOthersAgreeing);
    RUN_TEST(test_aboveOOB_sensor0AboveOthersDisagreeing);
    RUN_TEST(test_aboveOOB_sensor1AboveOthersDisagreeing);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveAgreeing);
    RUN_TEST(test_aboveOOB_sensor0InBoundsAllAgreeing);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveAgreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsAllAgreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing);
    RUN_TEST(test_aboveOOB_sensor2InBoundsOthersAboveAgreeing);
    RUN_TEST();
    RUN_TEST();
    RUN_TEST();
    RUN_TEST();
    RUN_TEST();

    // RUN_TEST();

    /*readTempSensor() Tests*/
    // RUN_TEST(test_sensor_readsNegativeTempCorrectly);

    return UNITY_END();
}