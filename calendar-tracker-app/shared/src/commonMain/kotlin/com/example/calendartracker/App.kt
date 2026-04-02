package com.example.calendartracker

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier

/**
 * Aplicación principal con navegación entre las dos pantallas
 */
@Composable
fun App() {
    val viewModel = remember { CalendarViewModel() }
    var currentPage by remember { mutableStateOf(Page.CALENDAR) }
    
    MaterialTheme {
        Surface(
            modifier = Modifier.fillMaxSize(),
            color = MaterialTheme.colorScheme.background
        ) {
            when (currentPage) {
                Page.CALENDAR -> CalendarScreen(
                    viewModel = viewModel,
                    onNavigateToTasks = { currentPage = Page.TASKS }
                )
                Page.TASKS -> TasksScreen(
                    viewModel = viewModel,
                    onNavigateToCalendar = { currentPage = Page.CALENDAR }
                )
            }
        }
    }
}

enum class Page {
    CALENDAR,
    TASKS
}
