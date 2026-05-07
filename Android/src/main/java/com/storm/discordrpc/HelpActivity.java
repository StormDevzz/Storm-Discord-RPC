package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;

// актив с подробной справкой
public class HelpActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_help);

        TextView helpContent = findViewById(R.id.helpContent);
        Button backButton = findViewById(R.id.backButton);

        // загружаем контент справки
        String helpText = getHelpContent();
        helpContent.setText(helpText);

        backButton.setOnClickListener(v -> finish());
    }

    private String getHelpContent() {
        return "📋 ПОЛНАЯ ИНСТРУКЦИЯ ПО ПОЛУЧЕНИЮ CLIENT ID\n\n" +
               "🔑 ЧТО ТАКОЕ CLIENT ID?\n" +
               "Client ID (Application ID) - это уникальный 18-значный идентификатор вашего Discord приложения.\n\n" +
               "📱 ШАГ 1: СОЗДАНИЕ ПРИЛОЖЕНИЯ\n" +
               "1. Перейдите на Discord Developer Portal\n" +
               "   🌐 discord.com/developers/applications\n\n" +
               "2. Войдите в свой Discord аккаунт\n" +
               "   • Если у вас нет аккаунта, создайте его\n" +
               "   • Подтвердите email, если потребуется\n\n" +
               "3. Нажмите \"New Application\" (сверху справа)\n" +
               "   • Введите название приложения\n" +
               "   • Например: \"My Storm RPC\"\n" +
               "   • Нажмите \"Create\"\n\n" +
               "📋 ШАГ 2: ПОЛУЧЕНИЕ CLIENT ID\n" +
               "1. В левом меню выберите \"General Information\"\n" +
               "2. Найдите раздел \"APPLICATION ID\"\n" +
               "3. Нажмите \"Copy\" чтобы скопировать ID\n" +
               "4. Client ID выглядит так: 123456789012345678\n\n" +
               "⚙️ ШАГ 3: НАСТРОЙКА RICH PRESENCE (опционально)\n" +
               "1. В левом меню выберите \"Rich Presence\"\n" +
               "2. Нажмите \"Add Rich Presence\"\n" +
               "3. Загрузите изображения для статуса\n" +
               "4. Настройте названия изображений\n" +
               "5. Нажмите \"Save Changes\"\n\n" +
               "✅ ПРОВЕРКА CLIENT ID\n" +
               "Правильный Client ID:\n" +
               "• Состоит только из цифр\n" +
               "• Ровно 18 символов\n" +
               "• Не начинается с нуля\n" +
               "• Пример: 987654321098765432\n\n" +
               "❌ ЧЕГО НЕ ДОЛЖНО БЫТЬ:\n" +
               "• Букв или символов\n" +
               "• Меньше или больше 18 цифр\n" +
               "• Пробелов или переносов строк\n" +
               "• Начинаться с нуля\n\n" +
               "🔗 ПОЛЕЗНЫЕ ССЫЛКИ:\n" +
               "• Discord Developer Portal:\n" +
               "  discord.com/developers/applications\n\n" +
               "• Rich Presence документация:\n" +
               "  discord.com/developers/docs/rich-presence\n\n" +
               "• Discord поддержка:\n" +
               "  support.discord.com\n\n" +
               "🎯 СОВЕТЫ:\n" +
               "• Сохраните Client ID в надежном месте\n" +
               "• Client ID - публичная информация, ее можно безопасно делиться\n" +
               "• Для каждого проекта создавайте отдельное приложение\n" +
               "• Используйте понятные названия приложений\n\n" +
               "⚠️ ПРОБЛЕМЫ И РЕШЕНИЯ:\n" +
               "• Не могу войти - проверьте подтверждение email\n" +
               "• Нет кнопки \"New Application\" - обновите страницу\n" +
               "• Client ID не работает - проверьте, что скопировали все 18 цифр\n" +
               "• Приложение не найдено - возможно, оно было удалено\n\n" +
               "📞 ПОДДЕРЖКА:\n" +
               "Если у вас возникли проблемы, обратитесь в Discord поддержку или\n" +
               "посетите форум разработчиков Discord.";
    }
}
