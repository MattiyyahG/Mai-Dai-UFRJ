# Base image with Python
FROM python:3.9-slim

# Set the working directory
WORKDIR /app

# Copy requirements (we will define it later)
COPY requirements.txt requirements.txt

# Install dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Copy the application files
COPY . .

# Expose the FastAPI port
EXPOSE 8000

# Command to run the application
CMD ["uvicorn", "sensor:app", "--host", "0.0.0.0", "--port", "8000"]
