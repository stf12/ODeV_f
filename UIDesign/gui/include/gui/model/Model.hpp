#ifndef MODEL_HPP
#define MODEL_HPP

#include <touchgfx/Utils.hpp>
#ifdef ODEV_F
#include "FreeRTOS.h"
#include "queue.h"
#endif

class ModelListener;

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

    uint8_t getCounter() const { return m_nCounter; }
    void setCounter(uint8_t nNewCounter);

    /**
     * Asynchronous increment of the counter. This function is task safe.
     *
     * @param nIncrement [IN] specifies the value to add to the counter.
     */
    void incrementCounter(uint8_t nIncrement);

#ifdef ODEV_F
    void sendChar();
    void moveCursor();
    void SetOutputQueue(QueueHandle_t xQueue);
#endif

protected:
    /**
     * Pointer to the currently active presenter.
     */
    ModelListener* modelListener;



private:
    uint8_t m_nCounter;
#ifdef ODEV_F
    QueueHandle_t m_xInputQueue;
    QueueHandle_t m_xOutputQueue; ///< to send HID report to the host via USB.
#endif
};

#endif /* MODEL_HPP */
