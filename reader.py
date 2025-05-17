from flask import Flask, request, jsonify
import subprocess
import json
import os
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

@app.route('/send-matrix', methods=['POST'])
def receive_matrix():
    data = request.get_json()
    if not data or 'matrix' not in data:
        return jsonify({"error": "Неверные данные"}), 400

    matrix = data['matrix']
    print("Получена матрица:", matrix)

    # Сохраняем матрицу для C++
    with open("matrix_input.json", "w") as f:
        json.dump(matrix, f)

    try:
        result = subprocess.run(["./template"], capture_output=True, text=True, check=True)
        output = result.stdout.strip()
        print("Ответ от C++:", output)

        # Здесь предполагается, что C++ вернул массив, например: [12, 34, 56]
        parsed_output = json.loads(output)

    except subprocess.CalledProcessError:
        return jsonify({"error": "Ошибка при запуске C++"}), 500
    except json.JSONDecodeError:
        return jsonify({"error": "Некорректный JSON от C++"}), 500

    # Возвращаем массив как есть (например: [12, 34, 56])
    return jsonify(parsed_output)

if __name__ == '__main__':
    app.run(debug=True)
