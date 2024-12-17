#pragma once
#include <pgmspace.h>

// Array of fortune cookie messages stored in flash memory
const char* const fortunes[] PROGMEM = {
    // Life and Wisdom
    "Life is a series of surprises waiting to be discovered",
    "The best way to predict the future is to create it",
    "A journey of a thousand miles begins with a single step",
    "Wisdom comes from experience, experience comes from mistakes",
    "Every cloud has a silver lining, patience reveals it",
    "The greatest adventure is the one that lies ahead",
    "Time is the wisest counselor of all",
    "Fortune favors the bold and the prepared mind",
    
    // Success and Opportunity
    "Success is not final, failure is not fatal",
    "Opportunities multiply as they are seized",
    "Your potential is limited only by your imagination",
    "The harder you work, the luckier you get",
    "Small steps lead to big achievements",
    "Today's preparation determines tomorrow's success",
    "The best time to plant a tree was 20 years ago, the second best time is now",
    "Success comes to those who dare to begin",
    
    // Happiness and Well-being
    "Happiness is found when you stop comparing yourself to other people",
    "The greatest happiness comes from the smallest acts of kindness",
    "Your smile will be your umbrella for life's rain",
    "Joy is not in things, it is in us",
    "The secret of happiness is not in doing what one likes, but in liking what one does",
    "A merry heart is better than a full purse",
    "Happiness is a choice that requires effort at times",
    
    // Relationships and Love
    "Love all, trust a few, do wrong to none",
    "The heart that gives, gathers",
    "A kind word will keep someone warm for years",
    "True friendship is a plant of slow growth",
    "The greatest gift is the gift of understanding",
    "To have a friend, be a friend",
    "Love is not about possession, it's about appreciation",
    
    // Personal Growth
    "Your mind is your greatest asset, invest in it daily",
    "Change is inevitable, growth is optional",
    "The only person you should try to be better than is who you were yesterday",
    "Learning is a treasure that will follow its owner everywhere",
    "The best teacher is experience, the worst teacher is regret",
    "Your future is created by what you do today, not tomorrow",
    
    // Courage and Perseverance
    "Courage is not the absence of fear, but the triumph over it",
    "Fall seven times, stand up eight",
    "The greatest glory in living lies not in never falling, but in rising every time we fall",
    "Persistence guarantees that results are inevitable",
    "The difference between try and triumph is just a little umph",
    
    // Peace and Balance
    "Peace comes from within, do not seek it without",
    "Balance is not something you find, it's something you create",
    "Tranquility is found in doing what is right",
    "Inner peace is the key to outer harmony",
    "Silence is a source of great strength",
    
    // Action and Initiative
    "Actions speak louder than words, but not nearly as often",
    "The early bird gets the worm, but the second mouse gets the cheese",
    "Don't wait for opportunity, create it",
    "The best time to take action is now",
    "Tomorrow is often the busiest day of the week",
    
    // Dreams and Aspirations
    "Your dreams are the blueprints of your destiny",
    "Shoot for the moon, even if you miss, you'll land among the stars",
    "Dream big and dare to fail",
    "Your aspirations are your possibilities",
    "The future belongs to those who believe in the beauty of their dreams",
    
    // Wisdom and Knowledge
    "Knowledge speaks, but wisdom listens",
    "The only true wisdom is in knowing you know nothing",
    "Education is not preparation for life; education is life itself",
    "The more you know, the more you realize you don't know",
    "Wisdom is not a product of schooling but of the lifelong attempt to acquire it",
    
    // Change and Adaptation
    "Change is the only constant in life",
    "Adapt or perish, now as ever, is nature's inexorable imperative",
    "The secret of change is to focus all of your energy not on fighting the old, but on building the new",
    "Life is change, growth is optional, choose wisely",
    
    // Patience and Timing
    "Patience is bitter, but its fruit is sweet",
    "Good things come to those who wait, but better things come to those who work for it",
    "Time is the most valuable thing a man can spend",
    "The two most powerful warriors are patience and time",
    
    // Prosperity and Abundance
    "Your prosperity will grow with your generosity",
    "Wealth is not about having a lot of money, it's about having a lot of options",
    "The greatest wealth is contentment with little",
    "True abundance is not about material possessions",
    
    // Direction and Purpose
    "The journey is the reward",
    "Not all who wander are lost",
    "Life is not about finding yourself, it's about creating yourself",
    "Your path is not for others to understand",
    
    // Mindfulness and Present Moment
    "Yesterday is history, tomorrow is a mystery, today is a gift",
    "The present moment is the only moment available to us",
    "Life is available only in the present moment",
    "Now is the only time we have",
    
    // Creativity and Innovation
    "Creativity is intelligence having fun",
    "Innovation distinguishes between a leader and a follower",
    "The only limit to our realization of tomorrow will be our doubts of today",
    "Imagination is more important than knowledge"
};

const int NUM_FORTUNES = sizeof(fortunes) / sizeof(fortunes[0]);

// Helper function to read fortune from flash
String getFortune(int index) {
    if (index < 0 || index >= NUM_FORTUNES) return "";
    return String((const char*)pgm_read_ptr(&fortunes[index]));
}
