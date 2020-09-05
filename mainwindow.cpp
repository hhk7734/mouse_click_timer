#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <Windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    x_scale          = (65535.0 / (screenWidth - 1));
    y_scale          = (65535.0 / (screenHeight - 1));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::left_click() {
    INPUT input;

    ZeroMemory(&input, sizeof(INPUT));
    input.type       = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    input.mi.dx      = target_x;
    input.mi.dy      = target_y;
    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input, sizeof(INPUT));
    input.type       = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input, sizeof(INPUT));
    input.type       = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void MainWindow::click_event() {
    FILETIME  ft;
    ULONGLONG current;

    while(true) {
        GetSystemTimePreciseAsFileTime(&ft);
        current = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime
                  + 324000000000;

        if((LONGLONG)(target_time - current) < 0) {
            left_click();
            break;
        }
    }

    target_time += 864000000000;
    GetSystemTimePreciseAsFileTime(&ft);
    current = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime
              + 324000000000;

    QTimer::singleShot(((LONGLONG)(target_time - current)) / 10000 - 100,
                       this,
                       &MainWindow::click_event);
}

void MainWindow::on_pushButton_clicked() {
    FILETIME  ft;
    ULONGLONG start, current, target;

    GetSystemTimePreciseAsFileTime(&ft);
    start = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime
            + 324000000000;
    start = start - (start % 864000000000);

    target_x = LONG(ui->x_pos->value() * x_scale);
    target_y = LONG(ui->y_pos->value() * y_scale);

    int hour     = ui->hour->value();
    int minute   = ui->minute->value();
    int second   = ui->second->value();
    int m_second = ui->m_second->value();
    int u_second = ui->u_second->value();
    int n_second = ui->n_second->value();

    target      = ((hour * 60) + minute) * 60 + second;
    target_time = start + target * 10000000 + m_second * 1000 + u_second * 10
                  + n_second / 100;


    GetSystemTimePreciseAsFileTime(&ft);
    current = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime
              + 324000000000;

    if((LONGLONG)(target_time - current) < 1000000) {
        target_time += 864000000000;
    }

    QTimer::singleShot(((LONGLONG)(target_time - current)) / 10000 - 100,
                       this,
                       &MainWindow::click_event);
}
