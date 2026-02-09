import pandas as pd
import matplotlib.pyplot as plt

# Load the data
df = pd.read_csv('build/iq_data.csv')

plt.figure(figsize=(12, 6))

# Plot 1: Magnitude (Column 3)
plt.subplot(2, 1, 1)
plt.plot(df['Magnitude'], color='blue')
plt.title('Signal Magnitude')
plt.ylabel('Level')
plt.grid(True)

# Plot 2: Strength Percentage (Column 4)
plt.subplot(2, 1, 2)
plt.plot(df['Strength'], color='red')
plt.title('Signal Strength (%)')
plt.ylabel('Percent')
plt.ylim(0, 100) # Keep it 0 to 100%
plt.grid(True)

plt.tight_layout()
plt.savefig('build/signal_plot.png')
print("Plot saved as 'signal_plot.png'")