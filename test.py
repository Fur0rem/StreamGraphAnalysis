import stream_graph_py

import inspect
print(inspect.getmembers(stream_graph_py))
classes = inspect.getmembers(stream_graph_py, inspect.isclass)
functions = inspect.getmembers(stream_graph_py, inspect.isfunction)
print(f"Classes: {classes}, Functions: {functions}")

# # Call the increment method
# test.increment()

# # Get the number
# number = test.get_number()
# print(number)  # Prints: 1

# Call the test_func_c function
result = stream_graph_py.test_func_c(5)
print(result)  # Prints: 7

# # Create a Test object
test = stream_graph_py.Test(8)
