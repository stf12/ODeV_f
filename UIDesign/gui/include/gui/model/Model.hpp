#ifndef MODEL_HPP
#define MODEL_HPP

#include <touchgfx/Utils.hpp>
#ifdef ODEV_F
#include "FreeRTOS.h"
#include "queue.h"
#include "ShortcutsDemonTask.h"
#endif

class ModelListener;

typedef enum EModelEventID_t {
  E_SET_COUNTER = 0,
  E_SET_PAGE =    1
}EModelEventID;

typedef struct ModelEvent_t {
  EModelEventID id;
  uint16_t nParam;
  uint8_t reserved;
} ModelEvent;

/**
 * The Model class defines the data model in the model-view-presenter paradigm.
 * The Model is a singular object used across all presenters. The currently active
 * presenter will have a pointer to the Model through deriving from ModelListener.
 *
 * The Model will typically contain UI state information that must be kept alive
 * through screen transitions. It also usually provides the interface to the rest
 * of the system (the backend). As such, the Model can receive events and data from
 * the backend and inform the current presenter of such events through the modelListener
 * pointer, which is automatically configured to point to the current presenter.
 * Conversely, the current presenter can trigger events in the backend through the Model.
 */
class Model
{
public:
    Model();

    /**
     * Sets the modelListener to point to the currently active presenter. Called automatically
     * when switching screen.
     */
    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    /**
     * This function will be called automatically every frame. Can be used to e.g. sample hardware
     * peripherals or read events from the surrounding system and inject events to the GUI through
     * the ModelListener interface.
     */
    void tick();

    void ShortcutsPage(uint8_t nShortcutsPage);

#ifdef ODEV_F
    void sendCopyShortcut();
    void sendCutShotcut();
    void sendPasteShortcut();
    void sendSnipShortcut();
    void moveCursor();
    void SetOutputQueue(QueueHandle_t xQueue);
    void SetShortcutsDemonTask(ShortcutsDemonTask *pxTask);
#endif

protected:
    /**
     * Pointer to the currently active presenter.
     */
    ModelListener* modelListener;



private:
    uint8_t m_nShortcutPage;
#ifdef ODEV_F
    QueueHandle_t m_xInputQueue;
    QueueHandle_t m_xOutputQueue; ///< to send HID report to the host via USB.
    ShortcutsDemonTask *m_pxShortcutsTask; ///< to send shortcuts.
#endif
};

#endif /* MODEL_HPP */
