#include "anticaptcha.h"

AntiCaptcha::AntiCaptcha(QObject *parent) : QObject(parent)
{
    this->http = new RHttp;
}

/**
 * @brief AntiCaptcha::resolve
 * @param siteKey
 * @return
 */
QString AntiCaptcha::resolve(QString siteKey, QString url)
{
    // Запросим решение каптчи
    QString id = this->createTask(siteKey, url);
    return "12345";
}

/**
 * https://forum.qt.io/topic/65874/create-json-using-qjsondocument/2
 * @brief NoCaptchaTaskProxyless::createTask
 * @param siteKey
 * @param url
 * @return
 */
QString AntiCaptcha::createTask(QString siteKey, QString url)
{
    QString param = this->createParams(siteKey, url);
    this->http->post("http://api.anti-captcha.com/createTask", param.toUtf8());
    QString inbuf = this->http->inbuffQString();

    QJsonDocument response = QJsonDocument::fromJson(inbuf.toUtf8());
    QJsonObject responseObject = response.object();
    int errorId = responseObject.value("errorId").toInt();
    this->setErrorId(errorId);
    QString taskId = responseObject.value("taskId").toString();
    this->setTaskId(taskId);

    if (errorId == 0) { // 0 - ошибок нет, свойства errorCode и errorDescription отсутствуют
        this->setErrorCode("");
        this->setErrorDescription("");
    } else { // 1 - код ошибки, информация об ошибке находится в свойстве errorCode и errorDescription
        this->setErrorCode(responseObject.value("errorCode").toString());
        this->setErrorDescription(responseObject.value("errorDescription").toString());
    }

    return taskId;
}

/**
 * https://anticaptcha.atlassian.net/wiki/spaces/API/pages/196670/getTaskResult
 * @brief AntiCaptcha::getTaskResult
 * @param clientKey
 * @return
 */
QString AntiCaptcha::getTaskResult(QString taskId)
{
    QJsonObject recordObject;
    recordObject.insert("clientKey", QJsonValue::fromVariant(this->_taskId));
    recordObject.insert("taskId", QJsonValue::fromVariant(taskId));
    QJsonDocument doc(recordObject);
    QString param = doc.toJson();

    this->http->post("https://api.anti-captcha.com/getTaskResult", param.toUtf8());
    QString inbuf = this->http->inbuffQString();

    QJsonDocument response = QJsonDocument::fromJson(inbuf.toUtf8());
    QJsonObject responseObject = response.object();
    int errorId = responseObject.value("errorId").toInt();
    this->setErrorId(errorId);
    QString status = responseObject.value("status").toString();
    QJsonValue text = responseObject.value("solution").toObject().value("text");
    QJsonValue url = responseObject.value("solution").toObject().value("url");
    QString cost = responseObject.value("cost").toString();
    QString ip = responseObject.value("ip").toString();
    QString createTime = responseObject.value("createTime").toString();
    QString endTime = responseObject.value("endTime").toString();
    QString solveCount = responseObject.value("solveCount").toString();

    if (errorId == 0) { // 0 - ошибок нет, свойства errorCode и errorDescription отсутствуют
        this->setErrorCode("");
        this->setErrorDescription("");
    } else { // 1 - код ошибки, информация об ошибке находится в свойстве errorCode и errorDescription
        this->setErrorCode(responseObject.value("errorCode").toString());
        this->setErrorDescription(responseObject.value("errorDescription").toString());
    }

}

/**
 * @brief AntiCaptcha::humanError
 * @param errorCode
 * @return
 */
QString AntiCaptcha::humanError(QString errorCode)
{
    QStringList errors;
    errors << "ERROR_KEY_DOES_NOT_EXIST" << "ERROR_NO_SLOT_AVAILABLE" << "ERROR_ZERO_CAPTCHA_FILESIZE" << "ERROR_TOO_BIG_CAPTCHA_FILESIZE" << "ERROR_ZERO_BALANCE"
           << "ERROR_IP_NOT_ALLOWED" << "ERROR_CAPTCHA_UNSOLVABLE" << "ERROR_BAD_DUPLICATES" << "ERROR_NO_SUCH_METHOD" << "ERROR_IMAGE_TYPE_NOT_SUPPORTED"
           << "ERROR_NO_SUCH_CAPCHA_ID" << "ERROR_EMPTY_COMMENT" << "ERROR_IP_BLOCKED" << "ERROR_TASK_ABSENT" << "ERROR_TASK_NOT_SUPPORTED"
           << "ERROR_INCORRECT_SESSION_DATA" << "ERROR_PROXY_CONNECT_REFUSED" << "ERROR_PROXY_CONNECT_TIMEOUT" << "ERROR_PROXY_READ_TIMEOUT" << "ERROR_PROXY_BANNED"
           << "ERROR_PROXY_TRANSPARENT" << "ERROR_RECAPTCHA_TIMEOUT" << "ERROR_RECAPTCHA_INVALID_SITEKEY" << "ERROR_RECAPTCHA_INVALID_DOMAIN"
           << "ERROR_RECAPTCHA_OLD_BROWSER" << "ERROR_TOKEN_EXPIRED" << "ERROR_PROXY_HAS_NO_IMAGE_SUPPORT" << "ERROR_PROXY_INCOMPATIBLE_HTTP_VERSION"
           << "ERROR_FACTORY_SERVER_API_CONNECTION_FAILED" << "ERROR_FACTORY_SERVER_BAD_JSON" << "ERROR_FACTORY_SERVER_ERRORID_MISSING"
           << "ERROR_FACTORY_SERVER_ERRORID_NOT_ZERO" << "ERROR_FACTORY_MISSING_PROPERTY" << "ERROR_FACTORY_PROPERTY_INCORRECT_FORMAT" << "ERROR_FACTORY_ACCESS_DENIED"
           << "ERROR_FACTORY_SERVER_OPERATION_FAILED" << "ERROR_FACTORY_PLATFORM_OPERATION_FAILED" << "ERROR_FACTORY_PROTOCOL_BROKEN" << "ERROR_FACTORY_TASK_NOT_FOUND"
           << "ERROR_FACTORY_IS_SANDBOXED" << "ERROR_PROXY_NOT_AUTHORISED" << "ERROR_FUNCAPTCHA_NOT_ALLOWED" << "ERROR_INVISIBLE_RECAPTCHA" << "ERROR_FAILED_LOADING_WIDGET";

    switch (errors.indexOf(errorCode)) {
    case 0: // ERROR_KEY_DOES_NOT_EXIST
        return "Авторизационный ключ не существует в системе или имеет неверный формат (длина не равняется 32 байтам)";
    case 1: // ERROR_NO_SLOT_AVAILABLE
        return "Нет свободных работников в данный момент, попробуйте позже либо повысьте свою максимальную ставку здесь";
    case 2: // ERROR_ZERO_CAPTCHA_FILESIZE
        return "Размер капчи которую вы загружаете менее 100 байт";
    case 3: // ERROR_TOO_BIG_CAPTCHA_FILESIZE
        return "Размер капчи которую вы загружаете более 500,000 байт";
    case 4: // ERROR_ZERO_BALANCE
        return "Баланс учетной записи ниже нуля или равен нулю";
    case 5: // ERROR_IP_NOT_ALLOWED
        return "Запрос с этого IP адреса с текущим ключом отклонен. Управление доступом по IP находится здесь";
    case 6: // ERROR_CAPTCHA_UNSOLVABLE
        return "5 разных работников не смогли разгадать капчу, задание остановлено";
    case 7: // ERROR_BAD_DUPLICATES
        return "Не хватило заданного количества дублей капчи для функции 100% распознавания.";
    case 8: // ERROR_NO_SUCH_METHOD
        return "Запрос в API выполнен на несуществующий метод";
    case 9: // ERROR_IMAGE_TYPE_NOT_SUPPORTED
        return "Формат капчи не распознан по EXIF заголовку либо не поддерживается. Допустимые форматы: JPG, GIF, PNG";
    case 10: // ERROR_NO_SUCH_CAPCHA_ID
        return "Капча с таким ID не была найдена в системе. Убедитесь что вы запрашиваете состояние капчи в течение 300 секунд после загрузки.";
    case 11: // ERROR_EMPTY_COMMENT
        return "Отсутствует комментарий в параметрах рекапчи версии API 1";
    case 12: // ERROR_IP_BLOCKED
        return "Доступ к API с этого IP запрещен из-за большого количества ошибок. Узнать причину можно здесь.";
    case 13: // ERROR_TASK_ABSENT
        return "Отсутствует задача в методе createTask.";
    case 14: // ERROR_TASK_NOT_SUPPORTED
        return "Тип задачи не поддерживается или указан не верно.";
    case 15: // ERROR_INCORRECT_SESSION_DATA
        return "Неполные или некорректные данные об эмулируемом пользователе. Все требуемые поля не должны быть пустыми.";
    case 16: // ERROR_PROXY_CONNECT_REFUSED
        return "Не удалось подключиться к прокси-серверу - отказ в подключении";
    case 17: // ERROR_PROXY_CONNECT_TIMEOUT
        return "Таймаут подключения к прокси-серверу";
    case 18: // ERROR_PROXY_READ_TIMEOUT
        return "Таймаут операции чтения прокси-сервера.";
    case 19: // ERROR_PROXY_BANNED
        return "Прокси забанен на целевом сервисе капчи";
    case 20: // ERROR_PROXY_TRANSPARENT
        return "Ошибка проверки прокси. Прокси должен быть не прозрачным, скрывать адрес конечного пользователя. В противном случае Google будет фильтровать запросы с IP нашего сервера.";
    case 21: // ERROR_RECAPTCHA_TIMEOUT
        return "Таймаут загрузки скрипта рекапчи, проблема либо в медленном прокси, либо в медленном сервере Google";
    case 22: // ERROR_RECAPTCHA_INVALID_SITEKEY
        return "Ошибка получаемая от сервера рекапчи. Неверный/невалидный sitekey.";
    case 23: // ERROR_RECAPTCHA_INVALID_DOMAIN
        return "Ошибка получаемая от сервера рекапчи. Домен не соответствует sitekey.";
    case 24: // ERROR_RECAPTCHA_OLD_BROWSER
        return "Для задачи используется User-Agent неподдерживаемого рекапчей браузера.";
    case 25: // ERROR_TOKEN_EXPIRED
        return "Провайдер капчи сообщил что дополнительный изменяющийся токен устарел. Попробуйте создать задачу еще раз с новым токеном.";
    case 26: // ERROR_PROXY_HAS_NO_IMAGE_SUPPORT
        return "Прокси не поддерживает передачу изображений с серверов Google";
    case 27: // ERROR_PROXY_INCOMPATIBLE_HTTP_VERSION
        return "Прокси не поддерживает длинные (длиной 2000 байт) GET запросы и не поддерживает SSL подключения";
    case 28: // ERROR_FACTORY_SERVER_API_CONNECTION_FAILED
        return "Не смогли подключиться к API сервера фабрики в течени 5 секунд.";
    case 29: // ERROR_FACTORY_SERVER_BAD_JSON
        return "Неправильный JSON ответ фабрики, что-то сломалось.";
    case 30: // ERROR_FACTORY_SERVER_ERRORID_MISSING
        return "API фабрики не вернул обязательное поле errorId";
    case 31: // ERROR_FACTORY_SERVER_ERRORID_NOT_ZERO
        return "Ожидали errorId = 0 в ответе API фабрики, получили другое значение.";
    case 32: // ERROR_FACTORY_MISSING_PROPERTY
        return "Значения некоторых требуемых полей в запросе к фабрике отсутствуют. Клиент должен прислать все требуемы поля.";
    case 33: // ERROR_FACTORY_PROPERTY_INCORRECT_FORMAT
        return "Тип значения не соответствует ожидаемому в структуре задачи фабрики. Клиент должен прислать значение с требуемым типом.";
    case 34: // ERROR_FACTORY_ACCESS_DENIED
        return "Доступ к управлению фабрикой принадлежит другой учетной записи. Проверьте свой ключ доступа.";
    case 35: // ERROR_FACTORY_SERVER_OPERATION_FAILED
        return "Общий код ошибки сервера фабрики.";
    case 36: // ERROR_FACTORY_PLATFORM_OPERATION_FAILED
        return "Общий код ошибки платформы.";
    case 37: // ERROR_FACTORY_PROTOCOL_BROKEN
        return "Ошибка в протоколе во время выполнения задачи фабрики.";
    case 38: // ERROR_FACTORY_TASK_NOT_FOUND
        return "Задача не найдена или недоступна для этой операции.";
    case 39: // ERROR_FACTORY_IS_SANDBOXED
        return "Фабрика находится в режиме песочницы, создание задач доступно только для владельца фабрики. Переведите фабрику в боевой режим, чтобы сделать ее доступной для всех клиентов.";
    case 40: // ERROR_PROXY_NOT_AUTHORISED
        return "Заданы неверные логин и пароль для прокси";
    case 41: // ERROR_FUNCAPTCHA_NOT_ALLOWED
        return "Заказчик не включил тип задач Funcaptcha Proxyless в панели клиентов - Настройки API. Все заказчики должны прочитать условия, пройти мини тест и подписать/принять форму до того как смогут использовать данный тип задач.";
    case 42: // ERROR_INVISIBLE_RECAPTCHA
        return "Обнаружена попытка решить невидимую рекапчу в обычном режиме. В случае возникновения этой ошибки вам ничего не нужно предпринимать, наша система через некоторое время начнет решать задачи с этим ключом в невидимом режиме. Просто шлите еще задачи с тем же ключом и доменом.";
    case 43: // ERROR_FAILED_LOADING_WIDGET
        return "Не удалось загрузить виджет капчи в браузере работника. Попробуйте прислать новую задачу.";
    default:
        return "Не удалось распознать ошибку.";
    }
}
