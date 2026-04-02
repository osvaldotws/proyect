package com.example.calendartracker

import androidx.compose.runtime.mutableStateOf
import java.time.LocalDate
import java.time.YearMonth

/**
 * ViewModel para manejar el estado de la aplicación
 */
class CalendarViewModel {
    
    private val _state = mutableStateOf(AppState())
    val state: AppState get() = _state.value
    
    fun toggleDay(date: LocalDate) {
        _state.value = _state.value.toggleDay(date)
    }
    
    fun selectDate(date: LocalDate?) {
        _state.value = _state.value.selectDate(date)
    }
    
    fun navigateToPreviousMonth() {
        _state.value = _state.value.navigateToMonth(_state.value.currentMonth.minusMonths(1))
    }
    
    fun navigateToNextMonth() {
        _state.value = _state.value.navigateToMonth(_state.value.currentMonth.plusMonths(1))
    }
    
    fun addTask(date: LocalDate, description: String) {
        if (description.isNotBlank()) {
            val task = Task(description = description.trim())
            _state.value = _state.value.addTaskToDate(date, task)
        }
    }
    
    fun removeTask(date: LocalDate, taskId: String) {
        _state.value = _state.value.removeTaskFromDate(date, taskId)
    }
    
    fun toggleTaskCompletion(date: LocalDate, taskId: String) {
        val entry = _state.value.markedDays[date] ?: return
        val task = entry.tasks.find { it.id == taskId } ?: return
        val updatedTask = task.copy(isCompleted = !task.isCompleted)
        _state.value = _state.value.updateTaskOnDate(date, updatedTask)
    }
    
    fun updateTaskDescription(date: LocalDate, taskId: String, newDescription: String) {
        val entry = _state.value.markedDays[date] ?: return
        val task = entry.tasks.find { it.id == taskId } ?: return
        val updatedTask = task.copy(description = newDescription.trim())
        _state.value = _state.value.updateTaskOnDate(date, updatedTask)
    }
}
