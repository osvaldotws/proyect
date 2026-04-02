package com.example.calendartracker

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import java.time.DayOfWeek
import java.time.LocalDate
import java.time.YearMonth
import java.time.format.TextStyle
import java.util.*

/**
 * Pantalla del Calendario - Primera página
 * Permite seleccionar días marcándolos en verde
 */
@Composable
fun CalendarScreen(
    viewModel: CalendarViewModel,
    onNavigateToTasks: () -> Unit
) {
    val state = viewModel.state
    val currentMonth = state.currentMonth
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        // Header con navegación de meses
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            IconButton(onClick = { viewModel.navigateToPreviousMonth() }) {
                Text("◀", style = MaterialTheme.typography.titleLarge)
            }
            
            Column(horizontalAlignment = Alignment.CenterHorizontally) {
                Text(
                    text = currentMonth.month.getDisplayName(TextStyle.FULL, Locale.getDefault()),
                    style = MaterialTheme.typography.headlineMedium,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = currentMonth.year.toString(),
                    style = MaterialTheme.typography.titleLarge
                )
            }
            
            IconButton(onClick = { viewModel.navigateToNextMonth() }) {
                Text("▶", style = MaterialTheme.typography.titleLarge)
            }
        }
        
        Spacer(modifier = Modifier.height(24.dp))
        
        // Contador de días marcados
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = MaterialTheme.colorScheme.primaryContainer)
        ) {
            Column(
                modifier = Modifier.padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text(
                    text = "Días Marcados",
                    style = MaterialTheme.typography.titleMedium
                )
                Text(
                    text = "${state.totalMarkedDays}",
                    style = MaterialTheme.typography.displayLarge,
                    fontWeight = FontWeight.Bold,
                    color = MaterialTheme.colorScheme.primary
                )
            }
        }
        
        Spacer(modifier = Modifier.height(24.dp))
        
        // Grid de días de la semana
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            DayOfWeek.values().forEach { dayOfWeek ->
                Box(
                    modifier = Modifier.weight(1f),
                    contentAlignment = Alignment.Center
                ) {
                    Text(
                        text = dayOfWeek.getDisplayName(TextStyle.SHORT, Locale.getDefault()),
                        style = MaterialTheme.typography.labelLarge,
                        fontWeight = FontWeight.Bold,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                }
            }
        }
        
        Spacer(modifier = Modifier.height(8.dp))
        
        // Grid del calendario
        val daysInMonth = getDaysInMonth(currentMonth)
        
        LazyVerticalGrid(
            columns = GridCells.Fixed(7),
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(4.dp),
            verticalArrangement = Arrangement.spacedBy(4.dp)
        ) {
            items(daysInMonth.size) { index ->
                val day = daysInMonth[index]
                CalendarDayCell(
                    day = day,
                    isCurrentMonth = day.month == currentMonth.month,
                    isMarked = state.markedDays.containsKey(day),
                    isToday = day == LocalDate.now(),
                    onClick = { viewModel.toggleDay(day) }
                )
            }
        }
        
        Spacer(modifier = Modifier.weight(1f))
        
        // Botón para navegar a la lista de tareas
        Button(
            onClick = onNavigateToTasks,
            modifier = Modifier
                .fillMaxWidth()
                .height(56.dp),
            enabled = state.totalMarkedDays > 0
        ) {
            Text(
                text = "Ver Lista de Días (${state.totalMarkedDays})",
                style = MaterialTheme.typography.titleMedium
            )
        }
    }
}

@Composable
private fun CalendarDayCell(
    day: LocalDate,
    isCurrentMonth: Boolean,
    isMarked: Boolean,
    isToday: Boolean,
    onClick: () -> Unit
) {
    val backgroundColor = when {
        isMarked -> Color(0xFF4CAF50) // Verde
        isToday -> MaterialTheme.colorScheme.primary.copy(alpha = 0.3f)
        !isCurrentMonth -> Color.Transparent
        else -> Color.Transparent
    }
    
    val textColor = when {
        isMarked -> Color.White
        !isCurrentMonth -> MaterialTheme.colorScheme.onSurface.copy(alpha = 0.3f)
        else -> MaterialTheme.colorScheme.onSurface
    }
    
    Box(
        modifier = Modifier
            .aspectRatio(1f)
            .clip(CircleShape)
            .background(backgroundColor)
            .clickable(onClick = onClick)
            .padding(4.dp),
        contentAlignment = Alignment.Center
    ) {
        Text(
            text = day.dayOfMonth.toString(),
            style = MaterialTheme.typography.bodyLarge,
            color = textColor,
            textAlign = TextAlign.Center
        )
    }
}

private fun getDaysInMonth(yearMonth: YearMonth): List<LocalDate> {
    val days = mutableListOf<LocalDate>()
    
    // Agregar días del mes anterior para completar la primera semana
    val firstDayOfMonth = yearMonth.atDay(1)
    val dayOfWeek = firstDayOfMonth.dayOfWeek
    val daysToPrepend = dayOfWeek.value - 1 // Lunes = 1
    
    for (i in daysToPrepend downTo 1) {
        days.add(firstDayOfMonth.minusDays(i.toLong()))
    }
    
    // Agregar todos los días del mes actual
    val daysInMonth = yearMonth.lengthOfMonth()
    for (day in 1..daysInMonth) {
        days.add(yearMonth.atDay(day))
    }
    
    // Agregar días del siguiente mes para completar el grid
    val remainingDays = 42 - days.size // 6 semanas x 7 días
    val lastDayOfMonth = yearMonth.atEndOfMonth()
    for (i in 1..remainingDays) {
        days.add(lastDayOfMonth.plusDays(i.toLong()))
    }
    
    return days
}
