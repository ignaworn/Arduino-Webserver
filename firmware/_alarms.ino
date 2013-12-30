#ifdef NTP_TIMER
    // Everyday at 7:30am
    void MorningAlarm() {
        // Master Parameter 0 ON
        SetParameters(0,0x01);
        // SlaveLed Parameter 0 ON
        #ifdef SLAVE_LED
            Slave.SendData('P',0x00,0x01);
        #endif
    }
#endif