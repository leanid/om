import std;
import glm;

int main()
{
    glm::mat4 m{}; // = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
    std::cout << "print from import glm;" << std::endl;
    std::cout << m[0][0] << std::endl;
    return std::cout.fail();
}
