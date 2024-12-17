#include <Arduino.h>
#include <HardwareSerial.h>
#include "fortunes.h"

// Define pins
const int CIRCUIT_PIN = 4;     // Input pin for circuit detection
const int PRINTER_RX = 32;     // Printer TX pin
const int PRINTER_TX = 33;     // Printer RX pin

// Initialize printer serial directly
HardwareSerial PrinterSerial(2);  // Use hardware serial 2

// State tracking
bool isPrinting = false;
bool lastCircuitState = HIGH;
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 2000;  // Minimum time between prints (2 seconds)

// Markov Chain structure
const int MAX_WORD_LENGTH = 12;    
const int MAX_NODES = 150;         
const int MAX_NEXT_WORDS = 5;      
const int MIN_FORTUNE_WORDS = 6;   
const int MAX_FORTUNE_WORDS = 12;  

struct MarkovNode {
    char word[MAX_WORD_LENGTH];
    struct NextWord {
        uint8_t nodeIndex;
        uint8_t count;
    } nextWords[MAX_NEXT_WORDS];
    uint8_t numNextWords;
    bool canStartSentence;    
    bool canEndSentence;
};

MarkovNode nodes[MAX_NODES];
uint16_t numNodes = 0;

// Printer commands
void printerInit() {
    // Initialize printer
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('@');  // Initialize command
    delay(50);
    
    // Set double size text
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(17);  // Double width and height (16 + 1)
    delay(50);
    
    // Set line spacing
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('3');  // Line spacing command
    PrinterSerial.write(45);  // Increased spacing for larger text
    delay(50);
}

void printerWrite(const char* str) {
    while (*str) {
        PrinterSerial.write(*str++);
        delay(2);  // Increased delay between characters
    }
}

void printerNewLine() {
    PrinterSerial.write('\r');
    PrinterSerial.write('\n');
    delay(50);  // Increased delay after line feed
}

void printerFeed(int lines) {
    for (int i = 0; i < lines; i++) {
        printerNewLine();
    }
}

// Find or create a node for a word
int findOrCreateNode(const char* word) {
    for (int i = 0; i < numNodes; i++) {
        if (strcmp(nodes[i].word, word) == 0) {
            return i;
        }
    }
    
    if (numNodes < MAX_NODES) {
        strncpy(nodes[numNodes].word, word, MAX_WORD_LENGTH - 1);
        nodes[numNodes].word[MAX_WORD_LENGTH - 1] = '\0';
        nodes[numNodes].numNextWords = 0;
        nodes[numNodes].canStartSentence = false;
        nodes[numNodes].canEndSentence = false;
        return numNodes++;
    }
    
    return -1;
}

void addTransition(const char* word1, const char* word2, bool isFirstWord, bool isLastWord) {
    int nodeIndex = findOrCreateNode(word1);
    if (nodeIndex == -1) return;
    
    int nextNodeIndex = findOrCreateNode(word2);
    if (nextNodeIndex == -1) return;
    
    if (isFirstWord) {
        nodes[nodeIndex].canStartSentence = true;
    }
    if (isLastWord) {
        nodes[nodeIndex].canEndSentence = true;
    }
    
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        if (nodes[nodeIndex].nextWords[i].nodeIndex == nextNodeIndex) {
            if (nodes[nodeIndex].nextWords[i].count < 255) {
                nodes[nodeIndex].nextWords[i].count++;
            }
            return;
        }
    }
    
    if (nodes[nodeIndex].numNextWords < MAX_NEXT_WORDS) {
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].nodeIndex = nextNodeIndex;
        nodes[nodeIndex].nextWords[nodes[nodeIndex].numNextWords].count = 1;
        nodes[nodeIndex].numNextWords++;
    }
}

void buildMarkovChain() {
    char wordBuffer[MAX_WORD_LENGTH];
    
    for (int i = 0; i < NUM_FORTUNES; i++) {
        String fortune = getFortune(i);
        
        int start = 0;
        int wordCount = 0;
        const char* words[15];
        
        for (int j = 0; j <= fortune.length(); j++) {
            if (j == fortune.length() || fortune[j] == ' ') {
                if (j - start < MAX_WORD_LENGTH) {
                    fortune.substring(start, j).toCharArray(wordBuffer, MAX_WORD_LENGTH);
                    words[wordCount++] = strdup(wordBuffer);
                    if (wordCount >= 15) break;
                }
                start = j + 1;
            }
        }
        
        for (int j = 0; j < wordCount - 1; j++) {
            addTransition(words[j], words[j + 1], j == 0, j == wordCount - 2);
        }
        
        for (int j = 0; j < wordCount; j++) {
            free((void*)words[j]);
        }
    }
}

const char* selectNextWord(int nodeIndex, const char* currentWord) {
    if (nodes[nodeIndex].numNextWords == 0) return nullptr;
    
    int totalWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {
            totalWeight += nodes[nodeIndex].nextWords[i].count;
        }
    }
    
    if (totalWeight == 0) return nullptr;
    
    int randomWeight = random(totalWeight);
    
    int currentWeight = 0;
    for (int i = 0; i < nodes[nodeIndex].numNextWords; i++) {
        const char* nextWord = nodes[nodes[nodeIndex].nextWords[i].nodeIndex].word;
        if (strcmp(nextWord, currentWord) != 0) {
            currentWeight += nodes[nodeIndex].nextWords[i].count;
            if (randomWeight < currentWeight) {
                return nextWord;
            }
        }
    }
    
    return nullptr;
}

String generateFortune() {
    int attempts = 0;
    int currentNode;
    do {
        currentNode = random(numNodes);
        attempts++;
    } while (!nodes[currentNode].canStartSentence && attempts < 50);
    
    String fortune = nodes[currentNode].word;
    int length = 1;
    
    while (length < MAX_FORTUNE_WORDS) {
        if ((length >= MIN_FORTUNE_WORDS && nodes[currentNode].canEndSentence && random(3) == 0) ||
            nodes[currentNode].numNextWords == 0) {
            break;
        }
        
        const char* nextWord = selectNextWord(currentNode, nodes[currentNode].word);
        if (!nextWord) break;
        
        fortune += " ";
        fortune += nextWord;
        length++;
        
        for (int i = 0; i < numNodes; i++) {
            if (strcmp(nodes[i].word, nextWord) == 0) {
                currentNode = i;
                break;
            }
        }
    }
    
    return fortune;
}

void printFortune(const String& fortune) {
    if (isPrinting) return;
    
    isPrinting = true;
    
    // Initialize printer with double size text
    printerInit();
    
    // Print with extra spacing
    printerFeed(2);
    
    // Print title with extra large text
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(49);  // Triple size (48 + 1)
    printerWrite("YOUR FORTUNE");
    printerFeed(2);
    
    // Reset to double size for fortune text
    PrinterSerial.write(27);  // ESC
    PrinterSerial.write('!');  // Text size command
    PrinterSerial.write(17);  // Double size (16 + 1)
    
    // Print fortune text with word wrapping
    char buffer[100];
    fortune.toCharArray(buffer, sizeof(buffer));
    
    // Split text into words and print with proper spacing
    char* word = strtok(buffer, " ");
    int lineLength = 0;
    
    while (word != NULL) {
        int wordLen = strlen(word);
        
        // Check if we need to start a new line
        if (lineLength + wordLen + 1 > 16) {  // 16 characters per line for double size text
            printerFeed(2);  // Extra space between lines
            lineLength = 0;
        }
        
        if (lineLength > 0) {
            printerWrite(" ");
            lineLength++;
        }
        
        printerWrite(word);
        lineLength += wordLen;
        
        word = strtok(NULL, " ");
    }
    
    // Add extra space at the end
    printerFeed(3);
    
    isPrinting = false;
}

void setup() {
    Serial.begin(115200);
    
    // Initialize printer serial with default settings
    PrinterSerial.begin(9600, SERIAL_8N1, PRINTER_RX, PRINTER_TX);
    delay(500);
    
    pinMode(CIRCUIT_PIN, INPUT_PULLUP);
    
    uint32_t seed = 0;
    for(int i = 0; i < 8; i++) {
        seed = (seed << 4) | (analogRead(0) & 0x0F);
        delay(1);
    }
    randomSeed(seed);
    
    buildMarkovChain();
    
    Serial.println("Setup complete!");
    Serial.printf("Initialized with %d nodes\n", numNodes);
    Serial.println("Ready for circuit trigger on GPIO4");
}

void loop() {
    bool currentState = digitalRead(CIRCUIT_PIN);
    unsigned long currentTime = millis();
    
    // Check for new print trigger
    if (currentState == LOW && lastCircuitState == HIGH && 
        !isPrinting && (currentTime - lastPrintTime >= PRINT_INTERVAL)) {
        
        String fortune = generateFortune();
        Serial.println("Circuit triggered - Printing fortune:");
        Serial.println(fortune);
        
        printFortune(fortune);
        lastPrintTime = currentTime;
    }
    
    lastCircuitState = currentState;
    delay(10);
}
