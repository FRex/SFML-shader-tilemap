#include <SFML/Graphics.hpp>
#include <array>


// NOTE: until samples are all filled once the FPS will be wrong
class FpsCounter {
public:
    int calculateFps() const
    {
        long long total = 0;
        for(long long sample : m_usecsamples) total += sample;
        return total ? ((1000000.0 * SampleCount) / total) : 0;
    }

    void addFrameMicroseconds(long long usec) { m_usecsamples[m_iter = (m_iter + 1) % SampleCount] = usec; }

private:
    enum { SampleCount = 60 };
    long long m_usecsamples[SampleCount] = {0x0};
    int m_iter = 0;

};

int main()
{
    sf::RenderWindow win(sf::VideoMode(800u, 600u), "Shader Tilemap");
    //win.setVerticalSyncEnabled(true);
    //win.setFramerateLimit(30u);

    const std::array<std::string, 4> shaders = {
        "shader.frag",
        "shaderx.frag",
        "shaderxy.frag",
        "shadery.frag",
    };

    int shaderNum = 0;

    sf::Texture graphics;
    graphics.loadFromFile("tilemap.png");
    graphics.setRepeated(true);

    sf::Image img;
    const int sidelen = 13;
    img.create(sidelen, sidelen, sf::Color::Black);
    for(int x = 0; x < img.getSize().x; ++x)
        for(int y = 0; y < img.getSize().y; ++y)
            img.setPixel(x, y, sf::Color(x, y, 0));

    sf::Texture data;
    data.loadFromImage(img);

    std::string loadedshader;

    sf::Shader shader;
    loadedshader = shaders[shaderNum];
    shader.loadFromFile(shaders[shaderNum], sf::Shader::Fragment);
    shader.setUniform("mapdata", data);
    shader.setUniform("tilegraphics", graphics);

    sf::Clock clo;
    FpsCounter counter;
    sf::Clock fpsclo;

    int iter = 0;
    bool rotation = false;

    sf::Transformable tran;
    tran.setPosition(sf::Vector2f(win.getSize()) / 2.f);

    const sf::Vector2f dataImageSize(data.getSize());
    tran.setOrigin(dataImageSize * 16.f);

    while(win.isOpen())
    {
        sf::Event eve;
        while(win.pollEvent(eve))
        {
            switch(eve.type)
            {
            case sf::Event::Closed:
                win.close();
                break;

            case sf::Event::MouseWheelScrolled:
            {
                sf::View view = win.getView();
                view.zoom(1.f - eve.mouseWheelScroll.delta / 10.f);
                win.setView(view);
                break;
            }

            case sf::Event::Resized:
            {
                win.setView(sf::View(sf::FloatRect(sf::Vector2f(), sf::Vector2f(eve.size.width, eve.size.height))));
                tran.setPosition(sf::Vector2f(win.getSize()) / 2.f);
                break;
            }

            case sf::Event::KeyPressed:
            {
                if(eve.key.code == sf::Keyboard::Right || eve.key.code == sf::Keyboard::D)
                {
                    shaderNum = (shaderNum + 1) % shaders.size();
                    loadedshader = shaders[shaderNum];
                    iter = 999123;
                    shader.loadFromFile(shaders[shaderNum], sf::Shader::Fragment);
                    shader.setUniform("mapdata", data);
                    shader.setUniform("tilegraphics", graphics);
                }

                if(eve.key.code == sf::Keyboard::Left || eve.key.code == sf::Keyboard::A)
                {
                    shaderNum = (shaderNum + shaders.size() - 1) % shaders.size();
                    loadedshader = shaders[shaderNum];
                    iter = 999123;
                    shader.loadFromFile(shaders[shaderNum], sf::Shader::Fragment);
                    shader.setUniform("mapdata", data);
                    shader.setUniform("tilegraphics", graphics);
                }

                if(eve.key.code == sf::Keyboard::R)
                    rotation = !rotation;

                if(eve.key.code == sf::Keyboard::T)
                    tran.setRotation(0.f);

                break;
            }

            } // switch
        } // while win poll event eve

        win.clear(sf::Color(0x3f3f3fff));
        const float rot = 90.f * clo.restart().asSeconds();
        tran.rotate(rotation ? rot : 0.f);

        sf::Vertex vert[4] = {
            sf::Vertex(sf::Vector2f(0.f, 0.f),sf::Vector2f(0.f, 0.f)),
            sf::Vertex(sf::Vector2f(0.f, 32.f * dataImageSize.y),sf::Vector2f(0.f, dataImageSize.y)),
            sf::Vertex(sf::Vector2f(32.f * dataImageSize.x, 0.f),sf::Vector2f(dataImageSize.x, 0.f)),
            sf::Vertex(32.f * sf::Vector2f(dataImageSize),sf::Vector2f(dataImageSize)),
        };

        const sf::RenderStates states(sf::BlendAlpha, tran.getTransform(), &data, &shader);
        win.draw(vert, std::size(vert), sf::TrianglesStrip, states);

        win.display();

        counter.addFrameMicroseconds(fpsclo.restart().asMicroseconds());

        if(iter++ >= 100) // update title every 100 frames
        {
            iter = 0;
            win.setTitle(loadedshader + " - " + std::to_string(counter.calculateFps()));
        }
    } // while win is open
}