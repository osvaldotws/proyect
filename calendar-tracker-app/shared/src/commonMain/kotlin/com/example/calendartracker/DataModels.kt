package com.example.calendartracker

import java.time.LocalDate

/**
 * Modelo de datos para representar un día con sus tareas
 */
data class DayEntry(
    val date: LocalDate,
    val tasks: List<Task> = emptyList(),
    val isMarked: Boolean = tasks.isNotEmpty()
) {
    fun addTask(task: Task): DayEntry {
        return copy(tasks = tasks + task, isMarked = true)
    }
    
    fun removeTask(taskId: String): DayEntry {
        return copy(tasks = tasks.filter { it.id != taskId }, isMarked = tasks.size > 1)
    }
    
    fun updateTask(updatedTask: Task): DayEntry {
        return copy(tasks = tasks.map { if (it.id == updatedTask.id) updatedTask else it })
    }
}

/**
 * Modelo de datos para una tarea
 */
data class Task(
    val id: String = java.util.UUID.randomUUID().toString(),
    val description: String,
    val isCompleted: Boolean = false,
    val createdAt: Long = System.currentTimeMillis()
)

/**
 * Estado de la aplicación
 */
data class AppState(
    val markedDays: Map<LocalDate, DayEntry> = emptyMap(),
    val selectedDate: LocalDate? = null,
    val currentMonth: java.time.YearMonth = java.time.YearMonth.now()
) {
    val totalMarkedDays: Int get() = markedDays.size
    
    fun toggleDay(date: LocalDate): AppState {
        return if (markedDays.contains(date)) {
            copy(markedDays = markedDays - date)
        } else {
            copy(markedDays = markedDays + (date to DayEntry(date)))
        }
    }
    
    fun selectDate(date: LocalDate?): AppState {
        return copy(selectedDate = date)
    }
    
    fun navigateToMonth(yearMonth: java.time.YearMonth): AppState {
        return copy(currentMonth = yearMonth)
    }
    
    fun addTaskToDate(date: LocalDate, task: Task): AppState {
        val entry = markedDays[date] ?: DayEntry(date)
        return copy(markedDays = markedDays + (date to entry.addTask(task)))
    }
    
    fun removeTaskFromDate(date: LocalDate, taskId: String): AppState {
        val entry = markedDays[date] ?: return this
        val updatedEntry = entry.removeTask(taskId)
        return if (updatedEntry.tasks.isEmpty()) {
            copy(markedDays = markedDays - date)
        } else {
            copy(markedDays = markedDays + (date to updatedEntry))
        }
    }
    
    fun updateTaskOnDate(date: LocalDate, updatedTask: Task): AppState {
        val entry = markedDays[date] ?: return this
        return copy(markedDays = markedDays + (date to entry.updateTask(updatedTask)))
    }
}
