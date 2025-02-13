import unittest
from hello.main import hello_world


class TestMain(unittest.TestCase):
    def test_hello_world(self):
        self.assertEqual(hello_world(), "Hello, World!")


if __name__ == "__main__":
    unittest.main()
