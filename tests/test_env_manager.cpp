#include "env_manager.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>    // for remove()
#include <cstdlib>   // for unsetenv()

// ----- Test for loading an environment file and reading variables -----
TEST(EnvManagerTest, LoadsEnvFileAndReadsVariable) {
    // Create a temporary .env file.
    std::string temp_env_file = "temp.env";
    std::ofstream ofs(temp_env_file);
    ASSERT_TRUE(ofs.is_open()) << "Failed to open temporary env file.";
    
    // Write environment variable definitions.
    ofs << "TEST_KEY=HelloWorld" << std::endl;
    // Add a comment and another variable.
    ofs << "# This is a comment line" << std::endl;
    ofs << "ANOTHER_KEY=42" << std::endl;
    ofs.close();

    // Clear the environment variables if already set.
    unsetenv("TEST_KEY");
    unsetenv("ANOTHER_KEY");

    // Create an EnvManager instance using the temporary file.
    EnvManager envManager(temp_env_file);

    // Verify that read_env returns the values from the file.
    EXPECT_EQ(envManager.read_env("TEST_KEY", "DefaultVal"), "HelloWorld")
        << "EnvManager should load TEST_KEY from the .env file.";
    EXPECT_EQ(envManager.read_env("ANOTHER_KEY", "DefaultVal"), "42")
        << "EnvManager should load ANOTHER_KEY from the .env file.";
    
    // Clean up: remove the temporary .env file.
    std::remove(temp_env_file.c_str());
}

// ----- Test for when an environment variable is not set -----
TEST(EnvManagerTest, ReadEnvReturnsDefaultWhenNotFound) {
    // Ensure that the variable is not set.
    unsetenv("NON_EXISTENT_KEY");

    // Use a non-existent file so that no variables are loaded.
    EnvManager envManager("nonexistent.env");

    // Verify that read_env returns the provided default value.
    EXPECT_EQ(envManager.read_env("NON_EXISTENT_KEY", "DefaultValue"), "DefaultValue")
        << "read_env should return the default value when the environment variable is not found.";
}