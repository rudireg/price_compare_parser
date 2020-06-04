#include "masterinstrument.h"

/**
 * @brief MasterInstrument::MasterInstrument
 * @param parent
 */
MasterInstrument::MasterInstrument(QObject *parent) : Parser(parent)
{
    this->siteUrl = "https://www.master-instrument.ru/";
}

/**
 * @brief MasterInstrument::search
 * @param article
 * @return
 */
QString MasterInstrument::search(Article &article)
{
    QString url = QString("https://www.master-instrument.ru/search/index.php?q=%1&s=Поиск").arg(article.article);
    article.sites[this->siteIndex].searchUrl = url;
    if (this->http->get(url)) {
        return this->http->inbuffQString();
    }
    return "";
}

/**
 * @brief MasterInstrument::findBlock
 * @param inbuf
 * @param article
 * @return
 */
QString MasterInstrument::findBlock(QString inbuf, QString article)
{
    QList<QString> blocks = this->splitBlocks(inbuf);
    if (blocks.isEmpty()) return "";
    // перебирая блоки - найти артикуль
    return this->findArticleInBlock(blocks, article);
}

/**
 * @brief MasterInstrument::parsePrice
 * @param block
 * @return
 */
float MasterInstrument::parsePrice(RString block)
{
    RString priceStr = block.cut_str_to_str("_price\">", "</span");
    if (priceStr.isEmpty()) return 0;
    if (priceStr.contains(",")) {
        priceStr = priceStr.cut_to_str(",");
    }
    QRegExp rx("(\\D+)");
    priceStr.replace(rx, "");
    if (priceStr.isEmpty()) return 0;
    float price = priceStr.toFloat();
    QString valute = block.cut_str_to_str("_price\">", "</span>");
    if (!valute.isEmpty()) {
        if (valute.contains("USD")) {
            price = price * this->usdRate() / 100;
        }
    }
    return price;
}

/**
 * @brief MasterInstrument::parseProductUrl
 * @param block
 * @param domain
 * @return
 */
QString MasterInstrument::parseProductUrl(RString block, QString domain)
{
    block = block.cut_str_to_str("product_name", "</div>");
    QString url = block.cut_str_to_str("<a href=\"", "\"");
    if (url.isEmpty()) return "";
    if (!url.startsWith("http")) {
        if (domain.endsWith("/")) {
            url.remove(0, 1);
        }
        url.prepend(domain);
    }
    return url;
}

/**
 * @brief MasterInstrument::splitBlocks
 * @param inbuf
 * @return
 */
QList<QString> MasterInstrument::splitBlocks(RString inbuf)
{
    QList<QString> blocks;
    QString startBlock = "<div class=\"bx_catalog_list_home";
    QString endBlock = "<div style=\"clear:both;\"></div>";
    if (!inbuf.contains(startBlock) || !inbuf.contains(endBlock)) {
        return blocks;
    }
    inbuf = inbuf.cut_str_to_str(startBlock, endBlock);
    QString boudoir = "<div class=\"product-wrapper";
    while (inbuf.contains(boudoir)) {
        inbuf.erase_to(boudoir);
        inbuf.erase_data(1);
        QString block;
        if (inbuf.contains(boudoir)) {
            block = inbuf.cut_to_str(boudoir);
        } else {
            block = inbuf;
        }
        if (!block.isEmpty()) {
            blocks.append(block);
        }
    }
    return blocks;
}

/**
 * @brief MasterInstrument::findArticleInBlock
 * @param blocks
 * @param article
 * @return
 */
QString MasterInstrument::findArticleInBlock(const QList<QString> blocks, QString article)
{
    foreach(QString block, blocks) {
        RString tmp = block;
        QString title = tmp.cut_str_to_str("product_name", "</div>");
        if (title.contains(article)) {
            return block;
        }
    }

    return "";
}
