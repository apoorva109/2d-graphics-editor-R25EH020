/* 
 * 2D Graphics Editor
 * Course: Programming in C (Semester 2 Project)
 * Description: A menu-driven command-line 2D Graphics Editor.
 *              Allows users to draw lines, rectangles, circles, and triangles
 *              on a 2D character array canvas. Shapes can be added, deleted,
 *              or modified using their unique IDs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Macro definitions for canvas limits and shape storage
#define ROWS 20
#define COLS 60
#define MAX_SHAPES 50

// Shape types
#define TYPE_LINE 1
#define TYPE_RECTANGLE 2
#define TYPE_CIRCLE 3
#define TYPE_TRIANGLE 4

// Structure to store coordinates and properties of a Shape
typedef struct {
    int id;
    int type;      // 1 = Line, 2 = Rectangle, 3 = Circle, 4 = Triangle
    int x1, y1;    // Primary coordinates (Line start, Rect top-left, Circle center, Tri vertex 1)
    int x2, y2;    // Secondary coordinates (Line end, Tri vertex 2)
    int x3, y3;    // Tertiary coordinates (Tri vertex 3)
    int width;     // Rectangle width
    int height;    // Rectangle height
    int radius;    // Circle radius
} Shape;

// Global variables
char canvas[ROWS][COLS];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_shape_id = 1; // Monotonically increasing ID for shapes

// Function declarations
void init_canvas();
void display_canvas();
void plot(int x, int y);
void draw_line(int x1, int y1, int x2, int y2);
void draw_rectangle(int x, int y, int w, int h);
void draw_circle(int cx, int cy, int r);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void redraw_canvas();
void clear_input_buffer();
void add_shape();
void delete_shape();
void modify_shape();
void list_shapes();

// Utility function to clear the input buffer to prevent infinite loops with scanf
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Fills the 2D array canvas with underscore characters '_'
void init_canvas() {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            canvas[i][j] = '_';
        }
    }
}

// Displays the canvas with column and row coordinates to make drawing easier
void display_canvas() {
    int i, j;
    
    printf("\n   ");
    // Print column numbers: tens digit
    for (j = 0; j < COLS; j++) {
        if (j % 10 == 0) {
            printf("%d", j / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n   ");
    // Print column numbers: units digit
    for (j = 0; j < COLS; j++) {
        printf("%d", j % 10);
    }
    printf("\n");

    // Print top border line
    printf("  +");
    for (j = 0; j < COLS; j++) {
        printf("-");
    }
    printf("+\n");

    // Print each row of the canvas with row indices on the side
    for (i = 0; i < ROWS; i++) {
        printf("%2d|", i);
        for (j = 0; j < COLS; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("|\n");
    }

    // Print bottom border line
    printf("  +");
    for (j = 0; j < COLS; j++) {
        printf("-");
    }
    printf("+\n\n");
}

// Plots a point safely on the canvas by checking coordinates limits
void plot(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = '*';
    }
}

/* 
 * Bresenham's Line Algorithm
 * Draws a line from (x1, y1) to (x2, y2) using integer arithmetic.
 * It avoids slow floating-point operations by accumulating decision errors.
 */
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (1) {
        plot(x1, y1);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draws a rectangle outline by drawing its four boundary edges
void draw_rectangle(int x, int y, int w, int h) {
    int i;
    // Draw top and bottom edges
    for (i = 0; i < w; i++) {
        plot(x + i, y);
        plot(x + i, y + h - 1);
    }
    // Draw left and right edges
    for (i = 0; i < h; i++) {
        plot(x, y + i);
        plot(x + w - 1, y + i);
    }
}

/* 
 * Midpoint Circle Algorithm
 * Draws a circle with center (cx, cy) and radius r.
 * Plots points in one octant and uses 8-way symmetry to draw the whole circle.
 */
void draw_circle(int cx, int cy, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r; // Initial decision parameter

    while (y >= x) {
        // Plot points in all 8 octants
        plot(cx + x, cy + y);
        plot(cx - x, cy + y);
        plot(cx + x, cy - y);
        plot(cx - x, cy - y);
        plot(cx + y, cy + x);
        plot(cx - y, cy + x);
        plot(cx + y, cy - x);
        plot(cx - y, cy - x);
        
        x++;
        // Update decision parameter and coordinate
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// Draws a triangle by connecting its three vertices with three lines
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Clears the canvas and redraws all active shapes
void redraw_canvas() {
    int i;
    init_canvas(); // Reset the canvas to underscores
    
    // Draw each active shape based on its stored type
    for (i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        if (s.type == TYPE_LINE) {
            draw_line(s.x1, s.y1, s.x2, s.y2);
        } else if (s.type == TYPE_RECTANGLE) {
            draw_rectangle(s.x1, s.y1, s.width, s.height);
        } else if (s.type == TYPE_CIRCLE) {
            draw_circle(s.x1, s.y1, s.radius);
        } else if (s.type == TYPE_TRIANGLE) {
            draw_triangle(s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
        }
    }
}

// Prints the details of all currently active shapes
void list_shapes() {
    int i;
    if (shape_count == 0) {
        printf("\n[Notice] No shapes added yet.\n");
        return;
    }
    
    printf("\n============= Active Shapes =============\n");
    for (i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("ID: %d | ", s.id);
        if (s.type == TYPE_LINE) {
            printf("Line from (%d, %d) to (%d, %d)\n", s.x1, s.y1, s.x2, s.y2);
        } else if (s.type == TYPE_RECTANGLE) {
            printf("Rectangle at (%d, %d) [Width: %d, Height: %d]\n", s.x1, s.y1, s.width, s.height);
        } else if (s.type == TYPE_CIRCLE) {
            printf("Circle at center (%d, %d) [Radius: %d]\n", s.x1, s.y1, s.radius);
        } else if (s.type == TYPE_TRIANGLE) {
            printf("Triangle with vertices (%d, %d), (%d, %d), (%d, %d)\n", s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
        }
    }
    printf("=========================================\n");
}

// Prompt the user to select and configure a new shape
void add_shape() {
    if (shape_count >= MAX_SHAPES) {
        printf("\n[Error] Cannot add shape. The storage is full!\n");
        return;
    }

    int choice;
    printf("\n--- Choose Shape to Add ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Go Back\n");
    printf("Enter choice (1-5): ");
    
    if (scanf("%d", &choice) != 1) {
        printf("\n[Error] Invalid menu choice.\n");
        clear_input_buffer();
        return;
    }

    if (choice < 1 || choice > 5) {
        printf("\n[Error] Choice out of range.\n");
        return;
    }
    if (choice == 5) {
        return; // Go back to the main menu
    }

    // Configure the new shape
    Shape s;
    s.id = next_shape_id;
    s.type = choice;

    // Helper validation flags
    int valid = 1;

    if (choice == TYPE_LINE) {
        printf("Enter start point (x1 y1): ");
        if (scanf("%d %d", &s.x1, &s.y1) != 2) valid = 0;
        printf("Enter end point (x2 y2): ");
        if (scanf("%d %d", &s.x2, &s.y2) != 2) valid = 0;
        
        // Warning if coordinates fall outside the canvas
        if (s.x1 < 0 || s.x1 >= COLS || s.y1 < 0 || s.y1 >= ROWS ||
            s.x2 < 0 || s.x2 >= COLS || s.y2 < 0 || s.y2 >= ROWS) {
            printf("[Warning] Some coordinates are out of canvas boundaries (%dx%d) and will be clipped.\n", COLS, ROWS);
        }
    } 
    else if (choice == TYPE_RECTANGLE) {
        printf("Enter top-left corner (x y): ");
        if (scanf("%d %d", &s.x1, &s.y1) != 2) valid = 0;
        printf("Enter width and height: ");
        if (scanf("%d %d", &s.width, &s.height) != 2) valid = 0;
        
        if (s.width <= 0 || s.height <= 0) {
            printf("[Error] Width and height must be positive numbers.\n");
            return;
        }
        if (s.x1 < 0 || s.x1 >= COLS || s.y1 < 0 || s.y1 >= ROWS) {
            printf("[Warning] Rectangle starting point is out of canvas limits.\n");
        }
    } 
    else if (choice == TYPE_CIRCLE) {
        printf("Enter center point (cx cy): ");
        if (scanf("%d %d", &s.x1, &s.y1) != 2) valid = 0;
        printf("Enter radius: ");
        if (scanf("%d", &s.radius) != 1) valid = 0;
        
        if (s.radius <= 0) {
            printf("[Error] Radius must be positive.\n");
            return;
        }
        if (s.x1 - s.radius < 0 || s.x1 + s.radius >= COLS || s.y1 - s.radius < 0 || s.y1 + s.radius >= ROWS) {
            printf("[Warning] Parts of the circle will lie outside canvas limits.\n");
        }
    } 
    else if (choice == TYPE_TRIANGLE) {
        printf("Enter vertex 1 (x1 y1): ");
        if (scanf("%d %d", &s.x1, &s.y1) != 2) valid = 0;
        printf("Enter vertex 2 (x2 y2): ");
        if (scanf("%d %d", &s.x2, &s.y2) != 2) valid = 0;
        printf("Enter vertex 3 (x3 y3): ");
        if (scanf("%d %d", &s.x3, &s.y3) != 2) valid = 0;

        if (s.x1 < 0 || s.x1 >= COLS || s.y1 < 0 || s.y1 >= ROWS ||
            s.x2 < 0 || s.x2 >= COLS || s.y2 < 0 || s.y2 >= ROWS ||
            s.x3 < 0 || s.x3 >= COLS || s.y3 < 0 || s.y3 >= ROWS) {
            printf("[Warning] Some vertices are outside the canvas limits.\n");
        }
    }

    if (!valid) {
        printf("\n[Error] Invalid numeric values entered.\n");
        clear_input_buffer();
        return;
    }

    // Add shape to list, increment counters, redraw
    shapes[shape_count] = s;
    shape_count++;
    next_shape_id++;
    redraw_canvas();
    printf("\n[Success] Shape added successfully with ID: %d\n", s.id);
}

// Allows deleting a shape by entering its unique ID
void delete_shape() {
    if (shape_count == 0) {
        printf("\n[Notice] No shapes to delete.\n");
        return;
    }

    list_shapes();
    int id;
    printf("Enter the ID of the shape you want to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("\n[Error] Invalid input.\n");
        clear_input_buffer();
        return;
    }

    int found_index = -1;
    int i;
    // Find the index of the shape with matching ID
    for (i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("\n[Error] Shape with ID %d was not found.\n", id);
        return;
    }

    // Shift elements left to remove the shape from the array
    for (i = found_index; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    
    // Redraw the canvas without the deleted shape
    redraw_canvas();
    printf("\n[Success] Shape ID %d deleted successfully.\n", id);
}

// Allows editing parameters of an existing shape by ID
void modify_shape() {
    if (shape_count == 0) {
        printf("\n[Notice] No shapes to modify.\n");
        return;
    }

    list_shapes();
    int id;
    printf("Enter the ID of the shape you want to modify: ");
    if (scanf("%d", &id) != 1) {
        printf("\n[Error] Invalid input.\n");
        clear_input_buffer();
        return;
    }

    int index = -1;
    int i;
    // Find shape index
    for (i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("\n[Error] Shape with ID %d not found.\n", id);
        return;
    }

    Shape *s = &shapes[index];
    int valid = 1;
    printf("\nModifying shape ID %d. Enter the new coordinate values below:\n", s->id);

    if (s->type == TYPE_LINE) {
        printf("Current values: Line from (%d, %d) to (%d, %d)\n", s->x1, s->y1, s->x2, s->y2);
        printf("Enter new start point (x1 y1): ");
        if (scanf("%d %d", &s->x1, &s->y1) != 2) valid = 0;
        printf("Enter new end point (x2 y2): ");
        if (scanf("%d %d", &s->x2, &s->y2) != 2) valid = 0;
    } 
    else if (s->type == TYPE_RECTANGLE) {
        printf("Current values: Rectangle at (%d, %d), width = %d, height = %d\n", s->x1, s->y1, s->width, s->height);
        printf("Enter new top-left corner (x y): ");
        if (scanf("%d %d", &s->x1, &s->y1) != 2) valid = 0;
        printf("Enter new width and height: ");
        if (scanf("%d %d", &s->width, &s->height) != 2) valid = 0;

        if (s->width <= 0 || s->height <= 0) {
            printf("[Error] Width and height must be positive. Modification aborted.\n");
            return;
        }
    } 
    else if (s->type == TYPE_CIRCLE) {
        printf("Current values: Circle at center (%d, %d), radius = %d\n", s->x1, s->y1, s->radius);
        printf("Enter new center point (cx cy): ");
        if (scanf("%d %d", &s->x1, &s->y1) != 2) valid = 0;
        printf("Enter new radius: ");
        if (scanf("%d", &s->radius) != 1) valid = 0;

        if (s->radius <= 0) {
            printf("[Error] Radius must be positive. Modification aborted.\n");
            return;
        }
    } 
    else if (s->type == TYPE_TRIANGLE) {
        printf("Current values: Triangle vertices (%d,%d), (%d,%d), (%d,%d)\n", s->x1, s->y1, s->x2, s->y2, s->x3, s->y3);
        printf("Enter new vertex 1 (x1 y1): ");
        if (scanf("%d %d", &s->x1, &s->y1) != 2) valid = 0;
        printf("Enter new vertex 2 (x2 y2): ");
        if (scanf("%d %d", &s->x2, &s->y2) != 2) valid = 0;
        printf("Enter new vertex 3 (x3 y3): ");
        if (scanf("%d %d", &s->x3, &s->y3) != 2) valid = 0;
    }

    if (!valid) {
        printf("\n[Error] Invalid inputs. Aborting shape modification.\n");
        clear_input_buffer();
        return;
    }

    // Refresh canvas
    redraw_canvas();
    printf("\n[Success] Shape ID %d modified successfully.\n", s->id);
}

// Main execution loop
int main() {
    int choice;
    init_canvas(); // Set up base canvas grid

    while (1) {
        printf("\n=== 2D Graphics Editor Menu ===\n");
        printf("1. Add Shape\n");
        printf("2. Delete Shape\n");
        printf("3. Modify Shape\n");
        printf("4. Display Canvas\n");
        printf("5. List Shapes\n");
        printf("6. Exit\n");
        printf("Enter choice (1-6): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Please enter a valid number.\n");
            clear_input_buffer();
            continue;
        }

        switch (choice) {
            case 1:
                add_shape();
                break;
            case 2:
                delete_shape();
                break;
            case 3:
                modify_shape();
                break;
            case 4:
                display_canvas();
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                printf("\nExiting program. Goodbye!\n");
                return 0;
            default:
                printf("\n[Error] Invalid option. Please select between 1 and 6.\n");
        }
    }

    return 0;
}
