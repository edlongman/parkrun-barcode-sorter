namespace ConsoleUart{

const uint16_t baud = 57600;

void init();
void enable();
void blockingSendLine(const char* buffer, const uint8_t length);
void scanLine(char* buffer, uint8_t length);
bool isScanComplete();

}