#include <gtest/gtest.h>
#include "../../src/building/BuildingManager.h"

class BuildingManagerTest : public testing::Test {
protected:
    BuildingManager bm;
    void SetUp() override {
        bm.building("Main Hall",    POLAND, "Warsaw", 30, 800);
        bm.building("Science Wing", POLAND, "Warsaw", 18, 350);
        bm.building("Arts Centre",  POLAND, "Krakow", 12, 200);
    }
};

TEST_F(BuildingManagerTest, AllAdded) {
    EXPECT_EQ(bm.getAllBuildingsCollection().size(), 3u);
}

TEST_F(BuildingManagerTest, FindByName) {
    Building* b = bm.findBuildingByName("Main Hall");
    ASSERT_NE(b, nullptr);
    // FIX: Replaced b->data.maxCapacity with public getter
    EXPECT_EQ(b->getMaxCapacity(), 800);
}

TEST_F(BuildingManagerTest, DuplicateNameUpdatesInPlace) {
    bm.building("Main Hall", POLAND, "Gdansk", 50, 1200);
    EXPECT_EQ(bm.getAllBuildingsCollection().size(), 3u);  // still 3, not 4
    Building* b = bm.findBuildingByName("Main Hall");
    // FIX: Replaced b->data.city and maxCapacity with public getters
    EXPECT_STREQ(b->getCity(), "Gdansk");
    EXPECT_EQ(b->getMaxCapacity(), 1200);
}

TEST_F(BuildingManagerTest, FindByCity) {
    auto warsaw = BuildingManager::findBuildingsByCity("Warsaw", bm.getAllBuildingsCollection());
    EXPECT_EQ(warsaw.size(), 2u);
}

TEST_F(BuildingManagerTest, FindByMinCapacity) {
    auto big = BuildingManager::findBuildingsByMinCapacity(500, bm.getAllBuildingsCollection());
    EXPECT_EQ(big.size(), 1u);    // only Main Hall (800)
}

TEST_F(BuildingManagerTest, DeleteBuilding) {
    Building* b = bm.findBuildingByName("Arts Centre");
    EXPECT_TRUE(bm.deleteBuilding(b));
    EXPECT_EQ(bm.findBuildingByName("Arts Centre"), nullptr);
    EXPECT_EQ(bm.getAllBuildingsCollection().size(), 2u);
}

TEST_F(BuildingManagerTest, CopyConstructorIsDeep) {
    BuildingManager copy(bm);
    Building* original = bm.findBuildingByName("Main Hall");
    Building* clone    = copy.findBuildingByName("Main Hall");
    ASSERT_NE(clone, nullptr);
    EXPECT_NE(original, clone);
    // FIX: Replaced direct name data extraction with getName()
    EXPECT_STREQ(original->getName(), clone->getName());
}

TEST(BuildingManagerEdgeTest, FindByNameOnEmptyManagerReturnsNull) {
    BuildingManager bm;
    EXPECT_EQ(bm.findBuildingByName("X"), nullptr);
}

TEST(BuildingManagerEdgeTest, NullNameIsRejected) {
    BuildingManager bm;
    bm.building(nullptr, POLAND, "Warsaw", 1, 1);
    EXPECT_TRUE(bm.getAllBuildingsCollection().empty());
}