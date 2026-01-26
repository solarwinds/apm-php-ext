#ifndef EXT_SETTINGS_SERVICE_C_WRAPPER_H
#define EXT_SETTINGS_SERVICE_C_WRAPPER_H

#define SETTINGS_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate a Solarwinds::SettingsService object
 * @param collector char*
 * @param service_key char*
 * @return
 */
void *Settings_Service_Allocate(char *collector, char *service_key);

/**
 * Free a Solarwinds::SettingsService object
 * @param service void*
 * @return
 */
void Settings_Service_Free(void *service);

/**
 * Get current settings from Solarwinds::SettingsService object
 * @param service void*
 * @param ans char*
 * @return bool
 */
bool Settings_Service_Get_Settings(void *service, char* ans);

#ifdef __cplusplus
}
#endif

#endif // EXT_SETTINGS_SERVICE_C_WRAPPER_H
