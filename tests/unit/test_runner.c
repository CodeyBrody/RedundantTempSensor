#include "unity.h"

void test_findAverageValidTemp_allSensorsValid(void);
void test_findAverageValidTemp_ignoresFaultySensor0(void);
void test_findAverageValidTemp_ignoresFaultySensor1(void);
void test_findAverageValidTemp_ignoresFaultySensor2(void);
void test_findAverageValidTemp_onlySensor2Valid(void);
void test_findAverageValidTemp_onlySensor0Valid(void);
void test_findAverageValidTemp_onlySensor1Valid(void);
void test_findAverageValidTemp_noValidSensorsReturnsNaN(void);

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
void test_aboveOOB_sensor0AboveOthersInBoundsAgreeing(void);
void test_aboveOOB_sensor1AboveOthersInBoundsAgreeing(void);
void test_aboveOOB_sensor2AboveOthersInBoundsAgreeing(void);
void test_aboveOOB_sensor0AboveOthersInBoundsDisagreeing(void);
void test_aboveOOB_sensor1AboveOthersInBoundsDisagreeing(void);
void test_aboveOOB_sensor2AboveOthersInBoundsDisagreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveAllAgreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveAllAgreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_sensor2InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor2InBoundsOthersAboveAllAgreeing(void);
void test_aboveOOB_sensor2InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_allSensorsAboveAgreeing(void);
void test_aboveOOB_allSensorsAboveDisagreeing(void);
void test_aboveOOB_testUpperBoundary(void);
void test_aboveOOB_sensor0InBoundsOthersAboveOneOutlier(void);
void test_aboveOOB_sensor1InBoundsOthersAboveOneOutlier(void);
void test_aboveOOB_sensor2InBoundsOthersAboveOneOutlier(void);
void test_aboveOOB_sensor0InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveAgreeing(void);
void test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing(void);

/*BELOW BOUNDS ERROR TEST DECLARATIONS*/
void test_belowOOB_sensor0BelowOthersInBoundsAgreeing(void);
void test_belowOOB_sensor1BelowOthersInBoundsAgreeing(void);
void test_belowOOB_sensor2BelowOthersInBoundsAgreeing(void);
void test_belowOOB_sensor0BelowOthersInBoundsDisagreeing(void);
void test_belowOOB_sensor1BelowOthersInBoundsDisagreeing(void);
void test_belowOOB_sensor2BelowOthersInBoundsDisagreeing(void);
void test_belowOOB_sensor0InBoundsOthersBelowAgreeing(void);
void test_belowOOB_sensor0InBoundsOthersBelowAllAgreeing(void);
void test_belowOOB_sensor0InBoundsOthersBelowDisagreeing(void);
void test_belowOOB_sensor1InBoundsOthersBelowAgreeing(void);
void test_belowOOB_sensor1InBoundsOthersBelowAllAgreeing(void);
void test_belowOOB_sensor1InBoundsOthersBelowDisagreeing(void);
void test_belowOOB_sensor2InBoundsOthersBelowAgreeing(void);
void test_belowOOB_sensor2InBoundsOthersBelowAllAgreeing(void);
void test_belowOOB_sensor2InBoundsOthersBelowDisagreeing(void);
void test_belowOOB_allSensorsBelowAgreeing(void);
void test_belowOOB_allSensorsBelowDisagreeing(void);
void test_belowOOB_testLowerBoundary(void);
void test_belowOOB_sensor0InBoundsOthersBelowOneOutlier(void);
void test_belowOOB_sensor1InBoundsOthersBelowOneOutlier(void);
void test_belowOOB_sensor2InBoundsOthersBelowOneOutlier(void);

/*ABOVE & BELOW BOUNDS TEST DECLARATIONS*/
void test_abv_bel_sensor0AboveSensor1BelowSensor2inBounds(void);
void test_abv_bel_sensor0AboveSensor2BelowSensor1inBounds(void);
void test_abv_bel_sensor1AboveSensor0BelowSensor2inBounds(void);
void test_abv_bel_sensor1AboveSensor2BelowSensor0inBounds(void);
void test_abv_bel_sensor2AboveSensor0BelowSensor1inBounds(void);
void test_abv_bel_sensor2AboveSensor1BelowSensor0inBounds(void);
void test_abv_bel_sensor0inBoundsSensor1BelowSensor2AboveOutlier(void);
void test_abv_bel_sensor0inBoundsSensor2AboveSensor1BelowOutlier(void);
void test_abv_bel_sensor1inBoundsSensor0BelowSensor2AboveOutlier(void);
void test_abv_bel_sensor1inBoundsSensor2AboveSensor0BelowOutlier(void);
void test_abv_bel_sensor2inBoundsSensor1BelowSensor0AboveOutlier(void);
void test_abv_bel_sensor2inBoundsSensor0AboveSensor1BelowOutlier(void);

/*USART COMMANDS TEST DECLARATIONS*/
void test_usart_setTimeCommandAccepted(void);
void test_usart_incompleteMessageIgnored(void);
void test_usart_unrecognizedMessageRejected(void);
void test_usart_validDateTimeSettingSuccessful(void);

/*TEMP SENSOR READ TEST DECLARATIONS*/
void test_sensor_readsMaxTempCorrectly(void);
void test_sensor_readsMinTempCorrectly(void);
void test_sensor_sensor0Missing(void);
void test_sensor_sensor1Missing(void);
void test_sensor_sensor2Missing(void);
void test_sensor_retriesFailedTransmit(void);
void test_sensor_retriesFailedReceive(void);
void test_sensor_retriesFailedReceiveAfterRetriedTransmit(void);
void test_sensor_failsAfterRetriedTransmit(void);
void test_sensor_failsAfterRetriedReceive(void);
void test_sensor_doesntRetrySuccessfulTransmitAndReceive(void);
void test_sensor_allSensorReadsMissingLogsErrors(void);
void test_sensor_readsNegativeTempCorrectly(void);

/*ERROR GUESSES TEST DECLARATIONS*/
void test_error_guesses_all0temps(void);
void test_error_guesses_bothBoundsAndMiddle(void);

void setUp(void) {}

void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_findAverageValidTemp_allSensorsValid);
    RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor0);
    RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor1);
    RUN_TEST(test_findAverageValidTemp_ignoresFaultySensor2);
    RUN_TEST(test_findAverageValidTemp_onlySensor2Valid);
    RUN_TEST(test_findAverageValidTemp_onlySensor0Valid);
    RUN_TEST(test_findAverageValidTemp_onlySensor1Valid);
    RUN_TEST(test_findAverageValidTemp_noValidSensorsReturnsNaN);

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
    /*ABOVE TESTS*/
    RUN_TEST(test_aboveOOB_sensor0AboveOthersInBoundsAgreeing);
    RUN_TEST(test_aboveOOB_sensor1AboveOthersInBoundsAgreeing);
    RUN_TEST(test_aboveOOB_sensor2AboveOthersInBoundsAgreeing);
    RUN_TEST(test_aboveOOB_sensor0AboveOthersInBoundsDisagreeing);
    RUN_TEST(test_aboveOOB_sensor1AboveOthersInBoundsDisagreeing);
    RUN_TEST(test_aboveOOB_sensor2AboveOthersInBoundsDisagreeing);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveAllAgreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveAllAgreeing);
    RUN_TEST(test_aboveOOB_sensor2InBoundsOthersAboveAgreeing);
    RUN_TEST(test_aboveOOB_sensor2InBoundsOthersAboveAllAgreeing);
    RUN_TEST(test_aboveOOB_sensor2InBoundsOthersAboveDisagreeing);
    RUN_TEST(test_aboveOOB_allSensorsAboveAgreeing);
    RUN_TEST(test_aboveOOB_allSensorsAboveDisagreeing);
    RUN_TEST(test_aboveOOB_testUpperBoundary);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveOneOutlier);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveOneOutlier);
    RUN_TEST(test_aboveOOB_sensor2InBoundsOthersAboveOneOutlier);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveAgreeing);
    RUN_TEST(test_aboveOOB_sensor0InBoundsOthersAboveDisagreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveAgreeing);
    RUN_TEST(test_aboveOOB_sensor1InBoundsOthersAboveDisagreeing);

    /*BELOW TESTS*/
    RUN_TEST(test_belowOOB_sensor0BelowOthersInBoundsAgreeing);
    RUN_TEST(test_belowOOB_sensor1BelowOthersInBoundsAgreeing);
    RUN_TEST(test_belowOOB_sensor2BelowOthersInBoundsAgreeing);
    RUN_TEST(test_belowOOB_sensor0BelowOthersInBoundsDisagreeing);
    RUN_TEST(test_belowOOB_sensor1BelowOthersInBoundsDisagreeing);
    RUN_TEST(test_belowOOB_sensor2BelowOthersInBoundsDisagreeing);
    RUN_TEST(test_belowOOB_sensor0InBoundsOthersBelowAgreeing);
    RUN_TEST(test_belowOOB_sensor0InBoundsOthersBelowAllAgreeing);
    RUN_TEST(test_belowOOB_sensor0InBoundsOthersBelowDisagreeing);
    RUN_TEST(test_belowOOB_sensor1InBoundsOthersBelowAgreeing);
    RUN_TEST(test_belowOOB_sensor1InBoundsOthersBelowAllAgreeing);
    RUN_TEST(test_belowOOB_sensor1InBoundsOthersBelowDisagreeing);
    RUN_TEST(test_belowOOB_sensor2InBoundsOthersBelowAgreeing);
    RUN_TEST(test_belowOOB_sensor2InBoundsOthersBelowAllAgreeing);
    RUN_TEST(test_belowOOB_sensor2InBoundsOthersBelowDisagreeing);
    RUN_TEST(test_belowOOB_allSensorsBelowAgreeing);
    RUN_TEST(test_belowOOB_allSensorsBelowDisagreeing);
    RUN_TEST(test_belowOOB_testLowerBoundary);
    RUN_TEST(test_belowOOB_sensor0InBoundsOthersBelowOneOutlier);
    RUN_TEST(test_belowOOB_sensor1InBoundsOthersBelowOneOutlier);
    RUN_TEST(test_belowOOB_sensor2InBoundsOthersBelowOneOutlier);

    /*ABOVE & BELOW TESTS*/
    RUN_TEST(test_abv_bel_sensor0AboveSensor1BelowSensor2inBounds);
    RUN_TEST(test_abv_bel_sensor0AboveSensor2BelowSensor1inBounds);
    RUN_TEST(test_abv_bel_sensor1AboveSensor0BelowSensor2inBounds);
    RUN_TEST(test_abv_bel_sensor1AboveSensor2BelowSensor0inBounds);
    RUN_TEST(test_abv_bel_sensor2AboveSensor0BelowSensor1inBounds);
    RUN_TEST(test_abv_bel_sensor2AboveSensor1BelowSensor0inBounds);
    RUN_TEST(test_abv_bel_sensor0inBoundsSensor1BelowSensor2AboveOutlier);
    RUN_TEST(test_abv_bel_sensor0inBoundsSensor2AboveSensor1BelowOutlier);
    RUN_TEST(test_abv_bel_sensor1inBoundsSensor0BelowSensor2AboveOutlier);
    RUN_TEST(test_abv_bel_sensor1inBoundsSensor2AboveSensor0BelowOutlier);
    RUN_TEST(test_abv_bel_sensor2inBoundsSensor1BelowSensor0AboveOutlier);
    RUN_TEST(test_abv_bel_sensor2inBoundsSensor0AboveSensor1BelowOutlier);

    /*USART COMMANDS TESTS*/
    RUN_TEST(test_usart_setTimeCommandAccepted);
    RUN_TEST(test_usart_incompleteMessageIgnored);
    RUN_TEST(test_usart_unrecognizedMessageRejected);
    RUN_TEST(test_usart_validDateTimeSettingSuccessful);

    /*TEMP SENSOR READ TESTS*/
    RUN_TEST(test_sensor_readsMaxTempCorrectly);
    RUN_TEST(test_sensor_readsMinTempCorrectly);
    RUN_TEST(test_sensor_sensor0Missing);
    RUN_TEST(test_sensor_sensor1Missing);
    RUN_TEST(test_sensor_sensor2Missing);
    RUN_TEST(test_sensor_retriesFailedTransmit);
    RUN_TEST(test_sensor_retriesFailedReceive);
    RUN_TEST(test_sensor_retriesFailedReceiveAfterRetriedTransmit);
    RUN_TEST(test_sensor_failsAfterRetriedTransmit);
    RUN_TEST(test_sensor_failsAfterRetriedReceive);
    RUN_TEST(test_sensor_doesntRetrySuccessfulTransmitAndReceive);
    RUN_TEST(test_sensor_allSensorReadsMissingLogsErrors);
    RUN_TEST(test_sensor_readsNegativeTempCorrectly);

    /*ERROR GUESSING TESTS*/
    RUN_TEST(test_error_guesses_all0temps);
    RUN_TEST(test_error_guesses_bothBoundsAndMiddle);

    return UNITY_END();
}