# Как получить Discord Client ID

## Что такое Client ID?

**Client ID** (также известный как Application ID) - это уникальный идентификатор вашего Discord приложения, который нужен для подключения к Discord Rich Presence API.

## Пошаговая инструкция

### 1. Перейдите в Discord Developer Portal
👉 **Прямая ссылка**: https://discord.com/developers/applications

### 2. Войдите в свой Discord аккаунт
- Нажмите "Login" в правом верхнем углу
- Введите свои данные для входа в Discord
- Если у вас нет аккаунта, создайте его

### 3. Создайте новое приложение
1. Нажмите кнопку **"New Application"** в правом верхнем углу
2. Введите название приложения (например: "Storm Discord RPC")
3. Нажмите **"Create"**

### 4. Получите Client ID
1. На странице приложения в левом меню выберите **"General Information"**
2. В разделе **"APPLICATION ID"** вы увидите ваш Client ID
3. Нажмите **"Copy"** чтобы скопировать ID

### 5. Настройте Rich Presence
1. В левом меню выберите **"Rich Presence"**
2. Нажмите **"Add Rich Presence"**
3. Настройте изображения и тексты (опционально)
4. Нажмите **"Save Changes"**

## Важные моменты

✅ **Client ID** - это 18-значное число (например: 123456789012345678)

✅ **Не путайте с Client Secret** - секрет нужен для OAuth2, но не для Rich Presence

✅ **Сохраните Client ID** - он понадобится для настройки приложения

## Пример Client ID
```
123456789012345678
```

## Дополнительные ссылки

- **Discord Developer Portal**: https://discord.com/developers/applications
- **Rich Presence Documentation**: https://discord.com/developers/docs/rich-presence/overview
- **Discord Support**: https://support.discord.com/

## Проверка Client ID

Client ID должен соответствовать следующим критериям:
- Состоит только из цифр
- Длина ровно 18 символов
- Не начинается с нуля

**Пример правильного Client ID**: `987654321098765432`

## Если что-то пошло не так

❌ **Не могу войти** - Убедитесь, что у вас есть подтвержденный Discord аккаунт

❌ **Нет кнопки "New Application"** - Проверьте, что вы вошли в правильный аккаунт

❌ **Не вижу Application ID** - Обновите страницу или попробуйте другой браузер

❌ **Client ID не работает** - Убедитесь, что вы скопировали все 18 цифр без пробелов

## Быстрый доступ

1. **Создать приложение**: https://discord.com/developers/applications
2. **Войти в Discord**: https://discord.com/login
3. **Документация**: https://discord.com/developers/docs/intro

---

📝 **Примечание**: Client ID - это публичная информация, ее можно безопасно делиться с другими.
