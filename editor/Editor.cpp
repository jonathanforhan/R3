#include "Editor.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QMessageBox>
#include <QScreen>
#include <QSlider>
#include <QWindow>
#include <QLabel>
#include <QVBoxLayout>

#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Math.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"

#include "components/CameraComponent.hpp"
#include "core/Mesh.hpp"
#include "core/Shader.hpp"
#include "core/Texture2D.hpp"
#include "systems/CameraSystem.hpp"
#include "LightCube.hpp"
#include "Cube.hpp"
#include "Vertices.hpp"

using namespace R3;

Editor::Editor(QWidget* parent)
    : QMainWindow(parent) {
    QRect rect{QGuiApplication::primaryScreen()->geometry()};
    qint32 w = rect.width() * 0.75;
    qint32 h = rect.height() * 0.75;
    qint32 x = (rect.width() - w) / 2;
    qint32 y = (rect.height() - h) / 2;
    setGeometry(x, y, w, h);

    createDockUI();
}

Editor::~Editor() {}

void Editor::createDockUI() {
    QDockWidget* dock = new QDockWidget(tr("R3 Light Editor"), this);
    dockWidget = new QWidget(dock);
    dockWidget->setLayout(new QVBoxLayout);
    dock->setWidget(dockWidget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void Editor::closeEvent(QCloseEvent* event) {
    QMessageBox::StandardButton e_quit =
        QMessageBox::question(this, "R3", tr("Are you sure you want to quit?\n"),
                              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

    if (e_quit != QMessageBox::Yes) {
        event->ignore();
        return;
    }

    event->accept();
    Engine::window().kill();
    emit finished();
}

void Editor::runEngine() {
    WId win_id = reinterpret_cast<WId>(Engine::window().nativeId());
    QWindow* win{QWindow::fromWinId(win_id)};
    QWidget* container = QWidget::createWindowContainer(win);

    setCentralWidget(container);
    show();

    Scene& defaultScene = Engine::addScene("default", true);

    Shader lightShader(ShaderType::GLSL, "shaders/light.vert", "shaders/light.frag");
    Shader defaultShader(ShaderType::GLSL, "shaders/default.vert", "shaders/default.frag");
    Texture2D crateTexture("textures/crate.png");
    Texture2D crateTextureSpecular("textures/crate_specular.png");
    Texture2D containerTexture("textures/container.jpg");
    Mesh cube(vertices);

    // crates
    Entity::create<Cube>(&defaultScene, vec3(3, 1, 3), vec3(1), defaultShader, cube, crateTexture, crateTextureSpecular, 128.0f);
    Entity::create<Cube>(&defaultScene, vec3(3, 1, -3), vec3(1), defaultShader, cube, crateTexture, crateTextureSpecular, 128.0f);
    Entity::create<Cube>(&defaultScene, vec3(-3, 1, 3), vec3(1), defaultShader, cube, crateTexture, crateTextureSpecular, 128.0f);
    Entity::create<Cube>(&defaultScene, vec3(-3, 1, -3), vec3(1), defaultShader, cube, crateTexture, crateTextureSpecular, 128.0f);

    // floor
    Entity::create<Cube>(&defaultScene, vec3(0), vec3(10, 1, 10), defaultShader, cube, containerTexture, containerTexture, 1.0f);

    QString labels[] = {
        tr("Blue Light"),
        tr("Green Light"),
        tr("Red Light"),
        tr("White Light"),
    };

    // lights
    Entity::create<LightCube>(&defaultScene, vec3(3, 4, 3), vec3(1), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(3, 4, -3), vec3(1, 0, 0), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(-3, 4, 3), vec3(0, 1, 0), lightShader, cube);
    Entity::create<LightCube>(&defaultScene, vec3(-3, 4, -3), vec3(0, 0, 1), lightShader, cube);

    int i = 0;
    for (auto&& [_, light] : Engine::activeScene().componentView<LightCube>().each()) {
        QSlider* sliders[3] = {
            new QSlider,
            new QSlider,
            new QSlider,
        };
        QWidget* sliderWidget = new QWidget;
        sliderWidget->setLayout(new QVBoxLayout);
        sliderWidget->layout()->addWidget(new QLabel(labels[i++]));
        for (int i = 0; i < 3; i++) {
            QSlider* curr = sliders[i];
            curr->setOrientation(Qt::Horizontal);
            curr->setMinimumWidth(300);
            curr->setRange(-11, 10);
            connect(curr, &QSlider::valueChanged, [&light, curr, i]() { light.position[i] = curr->value(); });
            curr->setValue(light.position[i]);
            sliderWidget->layout()->addWidget(curr);
        }
        dockWidget->layout()->addWidget(sliderWidget);
    }

    struct Player : Entity {
        void init() {
            emplace<CameraComponent>().setActive();
            Engine::activeScene().addSystem<CameraSystem>();
        }
    };

    Entity::create<Player>(&defaultScene);

    Engine::loop();
}
