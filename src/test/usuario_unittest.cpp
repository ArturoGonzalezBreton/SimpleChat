#include "../main/usuario.hpp"
#include "gtest/gtest.h"

TEST(constructor_test) {
  Usuario usuario("Arcadio Buendía");

  EXPECT_EQ("Arcadio Buendía", usuario.get_nombre());
}

TEST(get_nombre) {
  Usuario usuario("Arcadio Buendía");
  
  EXPECT_EQ("Arcadio Buendía", usuario.get_nombre());
}

TEST(set_nombre) {
  Usuario usuario();
  usuario.set_nombre("Arcadio Buendía");
  
  EXPECT_EQ("Arcadio Buendía", usuario.get_nombre());
}

int main(int argc, char ∗∗argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
