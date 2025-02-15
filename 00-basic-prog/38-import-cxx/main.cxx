import std;
import glm;

int main()
{
    using namespace std::numbers;
    glm::mat4 m{ 1.f }; // = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));
    glm::vec3 v{ 1.f };
    float     up  = glm::degrees(pi);
    float     pii = glm::radians(up);
    std::cout << "print from import glm;" << std::endl;
    std::cout << m[0][0] << std::endl;
    std::cout << v[0] << std::endl;
    std::cout << up << std::endl;
    std::cout << pii << std::endl;
    return std::cout.fail();
}
