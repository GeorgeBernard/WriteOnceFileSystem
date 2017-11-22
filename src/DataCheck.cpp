// Code to determine if different files systems are a match for each other


class DataCheck{
        public:
        // returns the fletcher checksum as a 32 bit value
        uint32_t fletcher32(const uint16_t *data, size_t len){
                uint32_t c0, c1;
                unsigned int i;

                for (c0 = c1 = 0; len >= 360; len -= 360) {
                        for (i = 0; i < 360; ++i) {
                                c0 = c0 + *data++;
                                c1 = c1 + c0;
                }
                c0 = c0 % 65535;
                c1 = c1 % 65535;
                }
                for (i = 0; i < len; ++i) {
                        c0 = c0 + *data++;
                        c1 = c1 + c0;
                }
                c0 = c0 % 65535;
                c1 = c1 % 65535;
                return (c1 << 16 | c0);
        }

        //returns if 2 checksums are a match fore each other
        bool checkSum(const uint16_t* data1, size_t len1, const uint16_t* data2, size_t len2){
                return fletcher32(data1, len1) == fletcher32(data2, len2);
        }

};


