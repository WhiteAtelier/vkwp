#include <gtest/gtest.h>
#include <vkwp/acceleration_structure_khr.hpp>
#include <vkwp/buffer.hpp>
#include <vkwp/command_buffer.hpp>
#include <vkwp/command_pool.hpp>
#include <vkwp/device.hpp>
#include <vkwp/device_memory.hpp>
#include <vkwp/instance.hpp>
#include <vkwp/queue.hpp>

TEST(VkwpInstanceTest, BasicTest)
{
    vkwp::Buffer                   buffer;
    vkwp::CommandBuffer            cmdbuf;
    vkwp::AccelerationStructureKHR acceleration_structure;
    vkwp::CommandPool              cmdpool;
    vkwp::DeviceMemory             mem;

    vkwp::Instance instance1;

    EXPECT_FALSE(instance1);
    instance1 = vkwp::Instance::create("Instance", "TestApp", 0, "TestEngine", 0, VK_API_VERSION_1_4, {}, {});
    EXPECT_TRUE(instance1);
    EXPECT_FALSE(!instance1);
    EXPECT_EQ(instance1.getLabel(), "Instance");
    EXPECT_FALSE(instance1.isExternal());
    EXPECT_EQ(static_cast<VkInstance>(instance1), *instance1.ptr());

    // Move Constructor
    vkwp::Instance instance2{ std::move(instance1) };
    EXPECT_TRUE(instance2);
    EXPECT_FALSE(!instance2);
    EXPECT_FALSE(instance2.isExternal());
    EXPECT_FALSE(instance1);
    EXPECT_TRUE(!instance1);
    EXPECT_FALSE(instance1.isExternal());

    // Move Assigner
    vkwp::Instance instance3;
    EXPECT_FALSE(instance3);
    instance3 = std::move(instance2);
    EXPECT_TRUE(instance3);
    EXPECT_FALSE(!instance3);
    EXPECT_FALSE(instance3.isExternal());
    EXPECT_FALSE(instance2);
    EXPECT_TRUE(!instance2);
    EXPECT_FALSE(instance2.isExternal());

    // Cast
    EXPECT_EQ(static_cast<VkInstance>(instance1), nullptr);
    EXPECT_EQ(static_cast<VkInstance>(instance2), nullptr);
    EXPECT_NE(static_cast<VkInstance>(instance3), nullptr);

    // As External
    vkwp::Instance external_instance1{ "Instance-External", instance3 };
    EXPECT_EQ(external_instance1.getLabel(), "Instance-External");
    EXPECT_TRUE(external_instance1.isExternal());
    EXPECT_EQ(static_cast<VkInstance>(instance3), static_cast<VkInstance>(external_instance1));

    // Move Constructor / External
    vkwp::Instance external_instance2{ std::move(external_instance1) };
    EXPECT_FALSE(external_instance1);
    EXPECT_FALSE(external_instance1.isExternal());
    EXPECT_TRUE(external_instance2);
    EXPECT_TRUE(external_instance2.isExternal());

    // Move Assign / External
    vkwp::Instance external_instance3;
    external_instance3 = std::move(external_instance2);
    EXPECT_FALSE(external_instance2);
    EXPECT_FALSE(external_instance2.isExternal());
    EXPECT_TRUE(external_instance3);
    EXPECT_TRUE(external_instance3.isExternal());

    // reset External
    external_instance3.reset();
    EXPECT_FALSE(external_instance3);
    EXPECT_TRUE(!external_instance3);
    EXPECT_FALSE(external_instance3.isExternal());
}

int
main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
