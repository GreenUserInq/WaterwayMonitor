using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using Formatting = Newtonsoft.Json.Formatting;

namespace DataPublisher.Common.Services
{
    /// <summary>
    /// Сохранение/Загрузка данных в Json формате
    /// </summary>
    public class JsonService
    {
        /// <summary>
        /// Метод для сохранения объекта в JSON файл
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="data"></param>
        /// <param name="filePath"></param>
        public static void SaveToJsonFile<T>(T data, string filePath)
        {
            try
            {
                // Сериализация объекта в строку JSON
                string json = JsonConvert.SerializeObject(data, Formatting.Indented);

                // Запись строки JSON в файл
                File.WriteAllText(filePath, json);

                Console.WriteLine("Данные успешно сохранены в файл.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ошибка при сохранении данных: {ex.Message}");
            }
        }


        /// <summary>
        /// Метод для извлечения объекта из JSON файла
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="filePath"></param>
        /// <returns></returns>
        public static T LoadFromJsonFile<T>(string filePath)
        {
            try
            {
                // Чтение строки JSON из файла
                string json = File.ReadAllText(filePath);

                // Десериализация строки JSON обратно в объект
                T data = JsonConvert.DeserializeObject<T>(json);

                Console.WriteLine("Данные успешно загружены из файла.");
                return data;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ошибка при загрузке данных: {ex.Message}");
                return default(T);
            }
        }
    }
}
