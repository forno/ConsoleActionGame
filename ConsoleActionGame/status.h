#pragma once

enum class state
{
    Title,
    Gaming,
    Result,
    Finish
};

struct game_data
{
    int score;
    int life_count;
};
