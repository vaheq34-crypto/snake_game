#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>
#include <QFile>
#include <QString>
#include <random>
#include <QDebug>
#include <QWidget>
#include <ranges>
#include <QPushButton>
#include <QVBoxLayout>
#include <iostream>
using namespace std;
const int c = 25;
int num = 0;
static QString loadStyleSheet(const char* filePath) {
  if (QFile file(filePath); file.open(QFile::ReadOnly | QFile::Text)) {
    return {file.readAll()};
  }
  return {};
}
enum class eventButton {
  right,
  left,
  up,
  down,
};
struct elem {
  QPoint point;
  eventButton event;
};
class snakeGame:public QWidget{
  deque<elem> body;
  mt19937 r{random_device{}()};
  uniform_int_distribution<int> X{0, 29};
  uniform_int_distribution<int> Y{0, 27};
public:
  snakeGame(){
    restart();
  };
  [[nodiscard]] auto getBody() const;
  void setBody(int newBody);
  void keyPressEvent(QKeyEvent *e) override;
  void forSnake();
  void restart();
  QPoint randomNumbers();
};

auto snakeGame::getBody() const {
  return body;
}
void snakeGame::setBody(int newBody) {
  switch (getBody()[newBody].event) {
    case eventButton::down:
    body[newBody].point.ry()+=1;
      break;
      case  eventButton::up:
      body[newBody].point.ry()-=1;
      break;
    case eventButton::right:
      body[newBody].point.rx()+=1;
      break;
    case  eventButton::left:
      body[newBody].point.rx()-=1;
      break;
  }
  if (newBody > 0)
    body[newBody-1].event = body[newBody].event;
}

void snakeGame::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
      if (body[body.size()-1].event != eventButton::down) {
        body[body.size()-1].event = eventButton::up;
      }
      break;
    case Qt::Key_Down:
    case Qt::Key_S:
      if (body[body.size()-1].event != eventButton::up) {
        body[body.size()-1].event = eventButton::down;
      }
      break;
    case Qt::Key_Left:
    case Qt::Key_A:
      if (body[body.size()-1].event != eventButton::right) {
        body[body.size()-1].event = eventButton::left;
      }
      break;
    case Qt::Key_Right:
    case Qt::Key_D:
      if (body[body.size()-1].event != eventButton::left) {
        body[body.size()-1].event = eventButton::right;
      }
      break;
    default: ;
  }
}
void snakeGame::forSnake() {
  elem tail = body.front();
  switch (tail.event) {
    case eventButton::down:  tail.point.ry() -= 1; break;
    case eventButton::up:    tail.point.ry() += 1; break;
    case eventButton::right: tail.point.rx() -= 1; break;
    case eventButton::left:  tail.point.rx() += 1; break;
  }
  body.push_front(tail);
}
QPoint snakeGame::randomNumbers() {
  return QPoint{X(r)*c,Y(r)*c};
}
void foo(snakeGame * window, vector<QLabel*>& snakeLabels,QPoint point) {
  auto* snake = new QLabel(window);
  snake->setAlignment(Qt::AlignCenter);
  snake->setObjectName("snake");
  snake->setFixedSize(c, c);
  snake->setGeometry(point.x() * c, point.y() * c, c, c);
  snakeLabels.push_back(snake);
  snake->show();
  snake->lower();
}
void snakeGame::restart() {
  body.clear();
  body.emplace_back(QPoint(2,14), eventButton::right);
  body.emplace_back(QPoint(3,14), eventButton::right);
  body.emplace_back(QPoint(4,14), eventButton::right);
}
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  snakeGame window;
  window.setFixedSize(30*c, 28*c);
  window.setWindowTitle("Snake Game");
  QLabel score("Score " + QString::number(num),&window);
  score.setObjectName("score");
  score.setGeometry(10,10,100,20);
  QRect screen = QGuiApplication::primaryScreen()->availableGeometry();
  window.move(
      screen.left() + (screen.width() - window.width()) / 2,
      screen.top() + (screen.height() - window.height()) / 2
  );
  QPoint appleLocation = QPoint(24*c,14*c);
  QLabel apple(&window);
  apple.setAlignment(Qt::AlignCenter);
  QFont font = apple.font();
  font.setPointSize(18);
  apple.setFont(font);
  apple.setGeometry(appleLocation.x(),appleLocation.y(),0,0);
  apple.setFixedSize(c,c);
  apple.setObjectName("apple");
  vector<QLabel*> snakeLabels;
  for (int i = 0;i<window.getBody().size();i++) {
   foo(&window,snakeLabels,window.getBody()[i].point);
  }
  auto time = new QTimer(&window);
  QObject::connect(time,&QTimer::timeout,[&]() {
    QPoint point = window.randomNumbers();

    for (int i = 0; i < window.getBody().size() && i < snakeLabels.size(); ++i) {
      window.setBody(i);
      snakeLabels[i]->setGeometry(window.getBody()[i].point.x() * c, window.getBody()[i].point.y() * c, 10, 10);
    }

    if (window.getBody().back().point.x() == appleLocation.x()/c && window.getBody().back().point.y() == appleLocation.y()/c) {
      window.forSnake();
      foo(&window,snakeLabels,window.getBody().front().point);
      appleLocation = point;
      apple.setGeometry(appleLocation.x(), appleLocation.y(), c, c);
      score.setText("Score " + QString::number(++num));
    }
     const bool hitWall = window.getBody().back().point.x() < 0 ||
           window.getBody().back().point.x() > 29 ||
           window.getBody().back().point.y() < 0 ||
           window.getBody().back().point.y() > 27;
     const bool hitSelf = ranges::any_of(window.getBody() | views::take(window.getBody().size() - 1), [&](const elem& part) {
         return part.point == window.getBody().back().point;
     });
     if ((hitWall || hitSelf) || snakeLabels.size() > 838) {
       auto* w = new QLabel(&window);
       w->setObjectName("notification");
       w->resize(200, 200);
       w->move((window.width() - w->width()) / 2, (window.height() - w->height()) / 2);
       auto* layout = new QVBoxLayout(w);
       layout->setAlignment(Qt::AlignCenter);
       auto* btn = new QPushButton("restart", w);
       layout->addWidget(new QLabel((snakeLabels.size() > 838)?"You won":"Game Over", w), 0, Qt::AlignCenter);
       layout->addWidget(btn);
        w->show();
       QObject::connect(btn, &QPushButton::clicked,[&,w]() {
         w->deleteLater();
         window.restart();
         for (auto* l : snakeLabels) {
          delete l;
         }
         snakeLabels.clear();
         for (auto& p : window.getBody()) {
          foo(&window, snakeLabels, p.point);
         }
         appleLocation = QPoint(24*c,14*c);
         apple.setGeometry(appleLocation.x(),appleLocation.y(),0,0);
         window.setFocus();
         num = 0;
         score.setText("Score " + QString::number(num));
         time->start(120);
          w->close();
       });

       time->stop();
     }
     window.update(); // NOLINT(*-misleading-indentation)
  });
  time->start(120);
  if (QString style = loadStyleSheet("style.css"); !style.isEmpty()) {
    app.setStyleSheet(style);
  }
  window.show();
  return QApplication ::exec();
}

