GroundIdle -> InFlight -> Chute -> Land


# GroundIdle
- Check no launch signal yet and check if its not moving
    - Stay there
- Switch states if signal happens
    - start collecting data

# InFlight
- Collect data
- If apogee switch state to Chute
    - check acceleration

## Unpowered flight
- check barometer, if it starts decreasing we reached apogee


# Chute
- When conditions is met, deploy chute
- When rocket stopped, switch state to Land

# Land
- Turn off systems to save battery
- end program / other things ppl want

# Collect data
- acceleration -> speed
- barometer / pressure
- timestamp
- store flight states

# crashes / hangs
- watchdog
