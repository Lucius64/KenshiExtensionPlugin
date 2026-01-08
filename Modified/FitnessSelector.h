// original https://github.com/KenshiReclaimer/KenshiLib
// Modified by Lucius on 2026-01-01:
// Added FitnessSelector(), int size(), void addItem(T, const float), T chooseAnItem(), T chooseAnItem_absoluteScore()

#pragma once

#include <map>
 
#include <ogre/OgreMemoryAllocatorConfig.h>
#include <kenshi/util/UtilityT.h>

// RVAs from FitnessSelector<CombatTechniqueData*>
template<typename T>
class FitnessSelector
{
public:
    // no_addr void FitnessSelector<CombatTechniqueData *>(const class FitnessSelector<CombatTechniqueData *> &);// public
    // no_addr void FitnessSelector<CombatTechniqueData *>();// public
    // no_addr void reset();// public
    // no_addr void deleteAll();// public
    // no_addr int size();// public
    // no_addr bool hasItem(class CombatTechniqueData * const);// public
    // no_addr bool empty();// public
    // no_addr float getItemScore(class CombatTechniqueData * const);// public
    // no_addr void addItem(class CombatTechniqueData *, const float);// public
    // no_addr bool removeItem(class CombatTechniqueData * const);// public
    // no_addr class CombatTechniqueData * chooseAnItem(class boost::random::mersenne_twister_engine<unsigned int,32,624,397,31,2567483615,11,4294967295,7,2636928640,15,4022730752,18,1812433253> &);// public
    // no_addr class CombatTechniqueData * chooseAnItem();// public
    // no_addr class CombatTechniqueData * chooseAnItem_absoluteScore();// public
    // no_addr class CombatTechniqueData * chooseHighest();// public
    // no_addr class CombatTechniqueData * takeHighest();// public
    // no_addr class CombatTechniqueData * takeRandom();// public
    std::map<float, T, std::less<float>, Ogre::STLAllocator<std::pair<float const, T>, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > > list; // 0x0 Member
    std::map<T, float, std::less<T>, Ogre::STLAllocator<std::pair<T const, float>, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > > itemsScores; // 0x28 Member
    float totalScore; // 0x50 Member
    float highestScore; // 0x54 Member
    T highestItem; // 0x58 Member
    // no_addr void ~FitnessSelector<CombatTechniqueData *>();// public
    // no_addr class FitnessSelector<CombatTechniqueData *> & operator=(const class FitnessSelector<CombatTechniqueData *> &);// public
    // no_addr void * __vecDelDtor(unsigned int);// public

    FitnessSelector()
        : list()
        , itemsScores()
        , totalScore(0.0f)
        , highestScore(-1.0f)
        , highestItem(NULL)
    {
    }

    int size()
    {
        return static_cast<int>(this->list.size());
    }

    void addItem(T item, const float score)
    {
        if (item != NULL && 0.0f < score)
        {
            this->totalScore += score;
            this->list[this->totalScore] = item;
            this->itemsScores[item] = score;
            if (this->highestScore < score)
            {
                this->highestScore = score;
                this->highestItem = item;
            }
        }
    }

    T chooseAnItem()
    {
        if (this->list.size() == 0)
            return NULL;
        else if (this->list.size() == 1)
            return this->list.begin()->second;

        float chooseScore = UtilityT::random(0.0f, this->totalScore);
        return this->list.upper_bound(chooseScore)->second;
    }

    T chooseAnItem_absoluteScore()
    {
        if (this->list.size() == 0)
            return NULL;

        float chooseScore = UtilityT::random(0.0f, 100.0f);
        if (chooseScore < this->totalScore)
            return this->list.upper_bound(chooseScore)->second;

        return NULL;
    }
};


template<>
void FitnessSelector<int>::addItem(int item, const float score)
{
    if (0.0f < score)
    {
        this->totalScore += score;
        this->list[this->totalScore] = item;
        this->itemsScores[item] = score;
        if (this->highestScore < score)
        {
            this->highestScore = score;
            this->highestItem = item;
        }
    }
}

template<>
void FitnessSelector<unsigned int>::addItem(unsigned int item, const float score)
{
    if (0.0f < score)
    {
        this->totalScore += score;
        this->list[this->totalScore] = item;
        this->itemsScores[item] = score;
        if (this->highestScore < score)
        {
            this->highestScore = score;
            this->highestItem = item;
        }
    }
}