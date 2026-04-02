package com.example.calendartracker

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import java.time.LocalDate
import java.time.format.DateTimeFormatter
import java.time.format.FormatStyle

/**
 * Pantalla de Lista de Días y Tareas - Segunda página
 * Muestra los días marcados y permite agregar/editar tareas
 */
@Composable
fun TasksScreen(
    viewModel: CalendarViewModel,
    onNavigateToCalendar: () -> Unit
) {
    val state = viewModel.state
    var newTaskText by remember { mutableStateOf("") }
    var expandedDay by remember { mutableStateOf<LocalDate?>(null) }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        // Header
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = "Días Marcados",
                style = MaterialTheme.typography.headlineMedium,
                fontWeight = FontWeight.Bold
            )
            
            IconButton(onClick = onNavigateToCalendar) {
                Text("📅", style = MaterialTheme.typography.titleLarge)
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // Resumen
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.secondaryContainer)
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Text(
                    text = "Total de días: ${state.totalMarkedDays}",
                    style = MaterialTheme.typography.titleMedium
                )
                val totalTasks = state.markedDays.values.sumOf { it.tasks.size }
                val completedTasks = state.markedDays.values.sumOf { it.tasks.count { task -> task.isCompleted } }
                Text(
                    text = "Tareas completadas: $completedTasks / $totalTasks",
                    style = MaterialTheme.typography.bodyMedium
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        if (state.markedDays.isEmpty()) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .wrapContentSize(Alignment.Center)
            ) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text(
                        text = "📭",
                        style = MaterialTheme.typography.displayLarge
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = "No hay días marcados",
                        style = MaterialTheme.typography.titleMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                    Text(
                        text = "Ve al calendario y marca algunos días",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                }
            }
        } else {
            // Lista de días marcados
            LazyColumn(
                modifier = Modifier.weight(1f),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                val sortedDays = state.markedDays.entries.sortedByDescending { it.key }
                
                items(sortedDays, key = { it.key }) { entry ->
                    val date = entry.key
                    val dayEntry = entry.value
                    
                    DayEntryCard(
                        date = date,
                        dayEntry = dayEntry,
                        isExpanded = expandedDay == date,
                        onExpandToggle = { 
                            expandedDay = if (expandedDay == date) null else date 
                        },
                        onAddTask = { description ->
                            viewModel.addTask(date, description)
                            newTaskText = ""
                        },
                        onRemoveTask = { taskId ->
                            viewModel.removeTask(date, taskId)
                        },
                        onToggleTask = { taskId ->
                            viewModel.toggleTaskCompletion(date, taskId)
                        }
                    )
                }
            }
        }
    }
}

@Composable
private fun DayEntryCard(
    date: LocalDate,
    dayEntry: DayEntry,
    isExpanded: Boolean,
    onExpandToggle: () -> Unit,
    onAddTask: (String) -> Unit,
    onRemoveTask: (String) -> Unit,
    onToggleTask: (String) -> Unit
) {
    var taskInput by remember { mutableStateOf("") }
    val formatter = DateTimeFormatter.ofLocalizedDate(FormatStyle.MEDIUM)
    
    Card(
        modifier = Modifier.fillMaxWidth(),
        onClick = onExpandToggle,
        colors = CardDefaults.cardColors(
            containerColor = if (isExpanded) 
                MaterialTheme.colorScheme.primaryContainer 
            else 
                MaterialTheme.colorScheme.surfaceVariant
        )
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            // Header del día
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column {
                    Text(
                        text = date.format(formatter),
                        style = MaterialTheme.typography.titleLarge,
                        fontWeight = FontWeight.Bold
                    )
                    Text(
                        text = "${dayEntry.tasks.size} tarea${if (dayEntry.tasks.size != 1) "s" else ""}",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                }
                
                Row(verticalAlignment = Alignment.CenterVertically) {
                    val completedCount = dayEntry.tasks.count { it.isCompleted }
                    if (dayEntry.tasks.isNotEmpty()) {
                        Text(
                            text = "$completedCount/${dayEntry.tasks.size}",
                            style = MaterialTheme.typography.labelLarge,
                            color = MaterialTheme.colorScheme.primary
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                    }
                    Icon(
                        imageVector = androidx.compose.material.icons.Icons.Default.ExpandMore,
                        contentDescription = if (isExpanded) "Contraer" else "Expandir",
                        modifier = Modifier.rotate(if (isExpanded) 180f else 0f)
                    )
                }
            }
            
            // Lista de tareas (solo si está expandido)
            if (isExpanded) {
                Spacer(modifier = Modifier.height(16.dp))
                
                if (dayEntry.tasks.isEmpty()) {
                    Text(
                        text = "No hay tareas para este día. Agrega una abajo.",
                        style = MaterialTheme.typography.bodyMedium,
                        color = MaterialTheme.colorScheme.onSurfaceVariant,
                        fontStyle = androidx.compose.ui.text.font.FontStyle.Italic
                    )
                } else {
                    dayEntry.tasks.forEach { task ->
                        TaskItem(
                            task = task,
                            onToggle = { onToggleTask(task.id) },
                            onDelete = { onRemoveTask(task.id) }
                        )
                        Spacer(modifier = Modifier.height(8.dp))
                    }
                }
                
                Spacer(modifier = Modifier.height(16.dp))
                
                // Input para nueva tarea
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.Bottom
                ) {
                    OutlinedTextField(
                        value = taskInput,
                        onValueChange = { taskInput = it },
                        modifier = Modifier
                            .weight(1f)
                            .heightIn(min = 56.dp),
                        placeholder = { Text("Nueva tarea...") },
                        keyboardOptions = KeyboardOptions(imeAction = ImeAction.Done),
                        keyboardActions = KeyboardActions(
                            onDone = {
                                if (taskInput.isNotBlank()) {
                                    onAddTask(taskInput)
                                    taskInput = ""
                                }
                            }
                        ),
                        singleLine = false,
                        maxLines = 3
                    )
                    
                    Spacer(modifier = Modifier.width(8.dp))
                    
                    Button(
                        onClick = {
                            if (taskInput.isNotBlank()) {
                                onAddTask(taskInput)
                                taskInput = ""
                            }
                        },
                        enabled = taskInput.isNotBlank(),
                        modifier = Modifier.height(56.dp)
                    ) {
                        Text("Agregar")
                    }
                }
            }
        }
    }
}

@Composable
private fun TaskItem(
    task: Task,
    onToggle: () -> Unit,
    onDelete: () -> Unit
) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .clickable(onClick = onToggle),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Checkbox(
            checked = task.isCompleted,
            onCheckedChange = { onToggle() }
        )
        
        Spacer(modifier = Modifier.width(8.dp))
        
        Text(
            text = task.description,
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.weight(1f),
            textDecoration = if (task.isCompleted) 
                androidx.compose.ui.text.style.TextDecoration.LineThrough 
            else 
                androidx.compose.ui.text.style.TextDecoration.None,
            color = if (task.isCompleted) 
                MaterialTheme.colorScheme.onSurfaceVariant 
            else 
                MaterialTheme.colorScheme.onSurface
        )
        
        IconButton(onClick = onDelete) {
            Icon(
                imageVector = androidx.compose.material.icons.Icons.Default.Delete,
                contentDescription = "Eliminar tarea",
                tint = MaterialTheme.colorScheme.error
            )
        }
    }
}

// Extensión para rotar iconos
@Composable
private fun Modifier.rotate(degrees: Float): Modifier {
    return this.then(
        androidx.compose.ui.Modifier.graphicsLayer {
            rotationZ = degrees
        }
    )
}
