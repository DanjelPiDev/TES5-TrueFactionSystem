#pragma once
#include "ArmorSlots.h"


namespace NPE {
    struct ArmorKeywordData {
        RE::FormID armorID;
        RE::FormID keywordID;
    };

    /**
     *
     */
    void Save(SKSE::SerializationInterface* a_intfc);

    /**
     *
     */
    void Load(SKSE::SerializationInterface* a_intfc);

    /**
     * For new game?
     */
    void Revert(SKSE::SerializationInterface* a_intfc);
}
