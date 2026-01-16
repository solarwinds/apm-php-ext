#ifndef EXT_SETTING_SERVICE_C_WRAPPER_H
#define EXT_SETTING_SERVICE_C_WRAPPER_H

#define SETTING_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate a Solarwinds::SettingService object
 * @param collector char*
 * @param service_key char*
 * @return
 */
void *Setting_Service_Allocate(char *collector, char *service_key);

/**
 * Free a Solarwinds::SettingService object
 * @param service void*
 * @return
 */
void Setting_Service_Free(void *service);

/**
 * Get current setting from Solarwinds::SettingService object
 * @param service void*
 * @param ans char*
 * @return bool
 */
bool Setting_Service_Get_Setting(void *service, char* ans);

#ifdef __cplusplus
}
#endif

#endif // EXT_SETTING_SERVICE_C_WRAPPER_H