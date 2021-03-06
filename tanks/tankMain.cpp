#include <cmath>
#include "../cpp/randgen.h"
#include "tank.h"
#include "../CMUgraphicsLib/CMUgraphics.h"

using namespace std;

void drawLandscape(window &, color skycolor, color groundcolor); // creates landscape. only called once
void getInput(window &, color skycolor, tank &left, tank &right, bool &leftTurn); // gets input for 2 parameters. returns when enter key (shoot) is pressed
color bullet(window &, color skycolor, tank &left, tank &right, bool &leftTurn); // goes through bullets path. returns color of what is hit.

void setBrushAndPenColor(window &w, color brushAndPenColor) {
    w.SetBrush(brushAndPenColor);
    w.SetPen(brushAndPenColor);
}

void main() {
    color whatIsHit = color(7, 7, 7); // set to a known, but useless value
    bool leftTurn = true;
    color skycolor = LIGHTBLUE;
    color groundcolor = WHITE;
    window w(1000, 900);
    w.SetBuffering(true);
    drawLandscape(w, skycolor, groundcolor); // only called once
    // tanks must be different colors!
    tank left(w, skycolor, groundcolor, color(5, 5, 5), true); // clubs symbol color
    tank right(w, skycolor, groundcolor, color(40, 40, 40), false); // left parentheses symbol color
    left.drawTank(w);
    right.drawTank(w);
    w.UpdateBuffer();

    while (whatIsHit != left.getColor() && whatIsHit != right.getColor()) { // until a tank is hit
        getInput(w, skycolor, left, right, leftTurn);
        whatIsHit = bullet(w, skycolor, left, right, leftTurn);
    }
    // clearing angle/power info
    setBrushAndPenColor(w, skycolor);
    w.DrawRectangle(0, 0, w.GetWidth(), 200, FILLED);
    // drawing winner message
    w.SetFont(50, PLAIN, ROMAN);
    setBrushAndPenColor(w, BLACK);
    if (whatIsHit == left.getColor()) {
        w.DrawString(w.GetWidth() / 2 - 75, w.GetHeight() / 2 - 66, "player 2 wins!");
    } else {
        w.DrawString(w.GetWidth() / 2 - 75, w.GetHeight() / 2 - 66, "player 1 wins!");
    }
    w.UpdateBuffer();
}

void drawLandscape(window &w, color skycolor, color groundcolor) { // creates landscape. only called once
    setBrushAndPenColor(w, skycolor);
    w.DrawRectangle(0, 0, w.GetWidth(), w.GetHeight(), FILLED);
    RandGen r;
    int ystart = r.RandInt((double) w.GetHeight() * 0.666, (double) w.GetHeight() * 0.75); // semi random height of ground near bottom of screen
    setBrushAndPenColor(w, groundcolor);
    for (double x = 0; x < w.GetWidth(); x++) {
        int yval = ystart + sin((double) (x / 60)) * 15; // ground will be sin curve
        w.DrawLine(x, yval, x, w.GetHeight());    // draws line from ground level to bottom of screen
    }
}

void getInput(window &w, color skycolor, tank &left, tank &right, bool &leftTurn) { // gets input for 2 parameters. returns when enter key (shoot) is pressed
    char k = '#';  // for key, set to a known, but useless value
    tank &currentTank = leftTurn ? left : right;
    int angle = currentTank.getAngle();
    int velocity = currentTank.getSpeed();
    w.SetFont(50, PLAIN, ROMAN);
    setBrushAndPenColor(w, BLACK);
    while (k != 13) { // return key
        setBrushAndPenColor(w, skycolor); // erasing up top
        w.DrawRectangle(0, 0, w.GetWidth(), 200, FILLED);
        w.SetFont(50, PLAIN, ROMAN); // redrawing up top
        setBrushAndPenColor(w, BLACK);
        w.DrawString(w.GetWidth() / 2 - 60, 100, "angle: " + to_string(angle));
        w.DrawString(w.GetWidth() / 2 - 60, 150, "speed: " + to_string(velocity));
        w.UpdateBuffer();

        // redraw tank
        setBrushAndPenColor(w, skycolor); // erasing around tank
        if (leftTurn) w.DrawRectangle(currentTank.getXstart() - 3, currentTank.getYstart() + 3, currentTank.getXend() + 3, currentTank.getYend() - 3, FILLED);
        else w.DrawRectangle(currentTank.getXstart() + 3, currentTank.getYstart() + 3, currentTank.getXend() - 3, currentTank.getYend() - 3, FILLED);

        // letting user change parameters (w/ limits)
        w.WaitKeyPress(k);
        if (k == 8 && angle < 90) angle++; // up
        if (k == 2 && angle > 1) angle--; // down
        if (k == 6 && velocity < 35) velocity++; // right
        if (k == 4 && velocity > 1) velocity--; // left

        currentTank.setAngle(angle);
        currentTank.setSpeed(velocity);
        currentTank.drawTank(w); // changes pen color
        w.UpdateBuffer();
    }
}

color bullet(window &w, color skycolor, tank &left, tank &right, bool &leftTurn) { // goes through bullets path. returns color of what is hit.
    setBrushAndPenColor(w, skycolor); // erasing up top to make sure it doesnt count hitting the text
    w.DrawRectangle(0, 0, w.GetWidth(), 200, FILLED);
    double dtime = 0.007;
    int bulletRadius = 4;
    int bulletX, bulletY;
    double radianAngle;
    tank &currentTank = leftTurn ? left : right;
    int angle = currentTank.getAngle();
    if (leftTurn) {
        radianAngle = angle * cdPi / 180;
        bulletX = currentTank.getXend() + 3;
    } else {
        radianAngle = ((180 - angle) * cdPi / 180);
        bulletX = currentTank.getXend() - 3;
    }
    int velocity = currentTank.getSpeed();
    bulletY = currentTank.getYend() - 3;
    leftTurn = !leftTurn;
    double xVel = cos(radianAngle) * velocity;
    double yVel = sin(radianAngle) * velocity;
    double time = 0;
    while (w.GetColor(bulletX, bulletY) == skycolor) {
        setBrushAndPenColor(w, BLACK);
        w.DrawCircle(bulletX, bulletY, bulletRadius, FILLED);
        w.UpdateBuffer();
        Sleep(1);
        setBrushAndPenColor(w, skycolor);
        w.DrawCircle(bulletX, bulletY, bulletRadius + 1, FILLED);
        time += dtime;
        bulletX += xVel * time;
        bulletY += -yVel * time + 40 * time * time;
    }
    return w.GetColor(bulletX, bulletY);
}