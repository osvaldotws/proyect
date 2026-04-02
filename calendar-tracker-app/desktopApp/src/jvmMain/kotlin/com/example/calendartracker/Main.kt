package com.example.calendartracker

import androidx.compose.ui.window.Window
import androidx.compose.ui.window.application
import androidx.compose.ui.window.rememberWindowState
import androidx.compose.ui.unit.dp

fun main() = application {
    Window(
        onCloseRequest = ::exitApplication,
        title = "Calendar Tracker",
        state = rememberWindowState(width = 400.dp, height = 700.dp)
    ) {
        App()
    }
}
