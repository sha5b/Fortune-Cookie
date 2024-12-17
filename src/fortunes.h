#pragma once
#include <pgmspace.h>

// Array of fortune cookie messages stored in flash memory
const char* const fortunes[] PROGMEM = {
    // Wisdom & Philosophy (Confucian-inspired)
    "The superior mind seeks truth, while the small mind seeks conformity",
    "Silence is the true friend that never betrays your secrets",
    "The journey of wisdom begins by calling things by their right names",
    "Learning without thought is labor lost, thought without learning perilous",
    "The noble soul has reverence without fear, the small soul fears blindly",
    "The gem cannot be polished without friction, nor man without trials",
    "When anger rises, think of the consequences that follow",
    "The wise find pleasure in water, the virtuous find pleasure in hills",
    
    // Life Insights
    "Experience is the lantern that illuminates the path behind us",
    "Destiny weaves the threads of chance into patterns of meaning",
    "Knowledge brightens the path, wisdom illuminates the journey",
    "The deepest rivers flow with the least sound and greatest force",
    "Time reveals truth as autumn reveals the stars at dawn",
    "The bamboo that bends is stronger than the oak that resists",
    "In the garden of life, patience bears the sweetest fruit",
    "The mirror reflects the face, but wisdom reflects the heart",
    
    // Personal Growth
    "To correct your faults is to guard your virtues for tomorrow",
    "The highest towers begin from the ground up with patience",
    "Excellence is not an act but a habit formed through time",
    "The journey of mastery begins with a single moment of choice",
    "True wisdom comes from knowing the extent of one's ignorance",
    "The superior person blames themselves, the inferior blames others",
    "Respect yourself and others will respect you naturally",
    
    // Success & Achievement
    "The greatest glory lies not in never falling but rising again",
    "Success comes to those who prepare well and leave nothing to chance",
    "Fortune favors the prepared mind and the resilient spirit",
    "The masterpiece of life is painted with strokes of persistence",
    "Achievement flows from the spring of determined action",
    "Victory belongs to those who believe in it the most enduringly",
    
    // Relationships & Harmony
    "Harmony in the home is the foundation of all prosperity",
    "The wise ruler governs their heart before leading others",
    "In relationships, respect is greater than affection alone",
    "True friendship multiplies joy and divides grief naturally",
    "The family that practices virtue creates lasting prosperity",
    
    // Character & Virtue
    "Virtue is not left to stand alone, noble neighbors will come",
    "The superior person acts before speaking, then speaks according",
    "To see what is right and not do it is want of courage",
    "The noble mind seeks to perfect their own character first",
    "Sincerity becomes apparent, goodness becomes powerful",
    
    // Time & Patience
    "Time is the wisest counselor, patience its truest student",
    "The seasons turn by heaven's law, success comes by wisdom",
    "Patience is bitter at first but sweet in its rewards",
    "Time reveals all truth and ripens all deep thoughts",
    
    // Knowledge & Learning
    "Study the past if you would define the future clearly",
    "Real knowledge is to know the extent of one's ignorance",
    "Learning without reflection leads not to true wisdom",
    "By three methods we learn wisdom: reflection, imitation, experience",
    
    // Inner Strength
    "The strongest walls are built within the mind and heart",
    "Inner peace comes from knowing one's true direction",
    "Strength flows not from physical capacity but indomitable will",
    "The noble spirit seeks harmony while lesser ones seek agreement",
    
    // Change & Adaptation
    "They who understand change shall prosper through all seasons",
    "Like water, the wise adapt to any situation naturally",
    "Change is constant, growth is optional, choose wisely",
    "The superior person adapts like water, yet maintains integrity",
    
    // Leadership & Influence
    "The leader is best when people barely know they exist",
    "Guide others by example rather than force of will",
    "True power comes from lifting others toward their potential",
    "Leadership begins with mastery of the self above all",
    
    // Balance & Moderation
    "The middle path leads to wisdom and lasting peace",
    "Balance in all things creates harmony in life",
    "Excess and deficiency are equally at fault always",
    "The wise maintain balance while others drift to extremes",
    
    // Purpose & Direction
    "The journey of a thousand miles reveals its purpose daily",
    "Your true nature manifests when you follow your path",
    "The noble mind seeks purpose beyond mere profit",
    "Direction comes from within, success from persistence"
};

const int NUM_FORTUNES = sizeof(fortunes) / sizeof(fortunes[0]);

// Helper function to read fortune from flash
String getFortune(int index) {
    if (index < 0 || index >= NUM_FORTUNES) return "";
    return String((const char*)pgm_read_ptr(&fortunes[index]));
}
