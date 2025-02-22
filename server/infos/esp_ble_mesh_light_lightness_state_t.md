# esp_ble_mesh_light_lightness_state_t

The `esp_ble_mesh_light_lightness_state_t` structure represents the state and behavior of a BLE Mesh lightness server. Each field corresponds to a specific property or feature of the lightness model as defined by the Bluetooth SIG Mesh specification. These states help control the light's brightness and behavior and allow applications (like mobile apps) to interact with and control BLE Mesh-enabled lighting devices.

---
```c
/** Parameters of Light Lightness state */
typedef struct {
    uint16_t lightness_linear;          /*!< The present value of Light Lightness Linear state */
    uint16_t target_lightness_linear;   /*!< The target value of Light Lightness Linear state */

    uint16_t lightness_actual;          /*!< The present value of Light Lightness Actual state */
    uint16_t target_lightness_actual;   /*!< The target value of Light Lightness Actual state */

    uint16_t lightness_last;            /*!< The value of Light Lightness Last state */
    uint16_t lightness_default;         /*!< The value of Light Lightness Default state */

    uint8_t  status_code;               /*!< The status code of setting Light Lightness Range state */
    uint16_t lightness_range_min;       /*!< The minimum value of Light Lightness Range state */
    uint16_t lightness_range_max;       /*!< The maximum value of Light Lightness Range state */
} esp_ble_mesh_light_lightness_state_t;
```
---
### **Field Descriptions and Uses**
1. **`lightness_linear`**
   - Represents the current lightness in a linear representation.
   - It’s useful for applications that need precise light control or smooth transitions.

2. **`target_lightness_linear`**
   - Indicates the desired (target) linear lightness value during a transition.

3. **`lightness_actual`**
   - Represents the current brightness level in a human-perceptible form (non-linear).
   - It directly corresponds to what the user perceives as brightness.

4. **`target_lightness_actual`**
   - Indicates the desired brightness level during a transition.

5. **`lightness_last`**
   - Stores the last non-zero brightness level.
   - It’s used to restore brightness to the previous value when turning the light back on.

6. **`lightness_default`**
   - The default brightness level when no specific target is set.

7. **`status_code`**
   - Indicates the result of a range setting operation (e.g., success or failure).
   - Useful for error handling and feedback in the app.

8. **`lightness_range_min`**
   - The minimum allowable brightness value.
   - Prevents the user from setting brightness too low.

9. **`lightness_range_max`**
   - The maximum allowable brightness value.
   - Prevents the user from setting brightness too high.

---

### **Representation in Mobile App UI**
A mobile app controlling BLE Mesh-enabled lighting might use the above fields in the following ways:

1. **Brightness Slider (lightness_actual and lightness_linear)**
   - A slider to adjust brightness, where:
     - The slider’s current position reflects `lightness_actual`.
     - The app might show `lightness_linear` in advanced settings for precise control.

2. **Transition Controls (target_lightness_actual and target_lightness_linear)**
   - The app could display options for gradual transitions (e.g., fade in/out).
   - A user could set a target brightness (`target_lightness_actual`).

3. **Last Brightness Button (lightness_last)**
   - A button to restore the light to the last non-zero brightness.
   - Useful for quickly returning to a previously set value.

4. **Default Brightness Option (lightness_default)**
   - A setting where the user can define a default brightness.
   - This value is applied whenever the light is turned on.

5. **Status Notifications (status_code)**
   - Displays messages such as "Brightness range updated successfully" or "Invalid range."

6. **Brightness Range Selector (lightness_range_min and lightness_range_max)**
   - Advanced settings allowing the user to define a minimum and maximum brightness.
   - These values might restrict the slider’s range.

7. **Visualization**
   - The app could visually represent brightness using icons (e.g., a sun symbol that changes intensity) or animations to reflect transitions.

---

### **Example App Flow**
1. **Main Screen**
   - Brightness slider.
   - Toggle to turn the light on/off.
   - "Restore Last Brightness" button.

2. **Advanced Settings**
   - Slider range settings (`lightness_range_min`, `lightness_range_max`).
   - Default brightness setting (`lightness_default`).
   - Transition duration selector for smooth fading effects.

3. **Notifications**
   - Success or error messages displayed when a range or default value is set, reflecting `status_code`.

---

### **Enhancements for UX**
- Include real-time feedback when the brightness changes.
- Provide an animation or progress indicator during transitions between current and target lightness values.
- Allow users to save and apply preset brightness levels.

This way, the app offers both simplicity for regular users and advanced control for power users, all while leveraging the features of the BLE Mesh Light Lightness Model.