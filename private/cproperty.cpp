/*
 * QkThings LICENSE
 * The open source framework and modular platform for smart devices.
 * Copyright (C) 2014 <http://qkthings.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cproperty.h"
#include "qhexspinbox.h"
#include "ptreeitemproperty.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QTimeEdit>

#include <QDebug>
#include <QFileDialog>
#include <QIcon>
#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>

CProperty::CProperty(QString label, Type type, CProperty *top) :
    QObject(top),
    m_label(label),
    m_type(type),
    m_valueWidget(0)
{
    if(top != 0)
        m_item = new pTreeItemProperty(this, top->item());
    else
        m_item = new pTreeItemProperty(this);
    m_item->setText(0, m_label);

    if(m_type == CProperty::Hex)
    {
        QHexSpinBox* spin = new QHexSpinBox();
        spin->setMinimum(0x80000000);
        spin->setMaximum(0x7FFFFFFF);

        m_valueWidget = spin;

        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    }
    else if(m_type == CProperty::Int)
    {
        QSpinBox* spin = new QSpinBox();
        spin->setMinimum(-99999999);
        spin->setMaximum(99999999);

        m_valueWidget = spin;
        connect(spin, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    }
    else if (m_type == CProperty::Double)
    {
        QDoubleSpinBox* spin = new QDoubleSpinBox();
        spin->setMinimum(-99999999);
        spin->setMaximum(99999999);
        spin->setDecimals(3); // float precision

        m_valueWidget = spin;
        connect(spin, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged()));
    }
    else if (m_type == CProperty::Bool)
    {
        QCheckBox *box = new QCheckBox();
        m_valueWidget = box;
        connect(box, SIGNAL(stateChanged(int)), this, SLOT(slotValueChanged()));

    }
    else if(m_type == CProperty::Enum)
    {
        QComboBox* box = new QComboBox();
        m_valueWidget = box;
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(slotValueChanged()));
    }
    else if(m_type == CProperty::Text)
    {
        QLineEdit* edit = new QLineEdit();
        m_valueWidget = edit;
        connect(edit, SIGNAL(returnPressed()), this, SLOT(slotValueChanged()));
    }
    else if(m_type == CProperty::Pixmap)
    {
        QPushButton* button = new QPushButton();
        button->setMinimumHeight(32);
        button->setIconSize(QSize(64, 32));
        button->setText("none");
        button->setFlat(true);
        m_valueWidget = button;
        connect(button, SIGNAL(clicked()), this, SLOT(slotValueChanged()));
    }
    else if(m_type == CProperty::Label)
    {
        QLabel* label = new QLabel();
        m_valueWidget = label;
    }
    else if(m_type == CProperty::DateTime)
    {
        QDateTimeEdit *edit = new QDateTimeEdit();
        edit->setDisplayFormat("hh:mm:ss yyyy/MM/dd");
        //edit->setDisplayFormat("dddd, d MMMM yy hh:mm:ss");
        m_valueWidget = edit;
    }
    else if(m_type == CProperty::Time)
    {
        QTimeEdit *edit = new QTimeEdit();
        edit->setDisplayFormat("hh:mm:ss");
        m_valueWidget = edit;
    }

    m_multiFunctionButton = new QToolButton();
    m_multiFunctionButton->hide();

    m_mainWidget = new QWidget();

    const int maxHeight = 18;
    m_valueWidget->setFixedHeight(maxHeight);
    m_multiFunctionButton->setFixedHeight(maxHeight);

    m_mainWidget->setLayout(new QHBoxLayout);
    m_mainWidget->layout()->setMargin(0);
    m_mainWidget->layout()->setSpacing(0);
    m_mainWidget->layout()->addWidget(m_valueWidget);
    m_mainWidget->layout()->addWidget(m_multiFunctionButton);
}

CProperty::~CProperty()
{


}

void CProperty::slotValueChanged()
{
    if(m_type == Pixmap)
    {
        QString fileName = QFileDialog::getOpenFileName(NULL, "Pixmap", "", "*.png *.jpg *.jpeg");
        if(fileName.isEmpty()) return;

        fileName.remove(0, QApplication::applicationDirPath().size()+1);

        QPushButton* button = reinterpret_cast<QPushButton*>(m_valueWidget);
        button->setIcon(QIcon(fileName));
        button->setText(fileName);
    }

    emit valueChanged(this);
}

void CProperty::setEnabled(bool enabled)
{
    m_valueWidget->setEnabled(enabled);
}

void CProperty::setLabel(QString label)
{
    m_label =  label;
}

void CProperty::setValue(QVariant value)
{
    if(m_type == CProperty::Hex)
    {
        QHexSpinBox* spin = reinterpret_cast<QHexSpinBox*>(m_valueWidget);
        spin->setValue(value.toUInt());
    }
    else if(m_type == CProperty::Int)
    {
        QSpinBox* spin = reinterpret_cast<QSpinBox*>(m_valueWidget);
        spin->setValue(value.toInt());
    }
    else if (m_type == CProperty::Double)
    {
        QDoubleSpinBox* spin = reinterpret_cast<QDoubleSpinBox*>(m_valueWidget);
        spin->setValue(value.toDouble());
    }
    else if (m_type == CProperty::Bool)
    {
        QCheckBox* box = reinterpret_cast<QCheckBox*>(m_valueWidget);
        box->setChecked(value.toBool());
    }
    else if(m_type == CProperty::Enum)
    {
        QComboBox* box = reinterpret_cast<QComboBox*>(m_valueWidget);
        box->setCurrentIndex(value.toInt());

    }
    else if(m_type == CProperty::Text)
    {
        QLineEdit* edit = reinterpret_cast<QLineEdit*>(m_valueWidget);
        edit->setText(value.toString());
    }
    else if(m_type == CProperty::Pixmap)
    {
        QPushButton* button = reinterpret_cast<QPushButton*>(m_valueWidget);
        button->setIcon(QIcon(value.toString()));
        button->setText(value.toString());
    }
    else if(m_type == CProperty::Label)
    {
        QLabel* label = reinterpret_cast<QLabel*>(m_valueWidget);
        label->setText(value.toString());
    }
    else if(m_type == CProperty::DateTime)
    {
        QDateTimeEdit* edit = reinterpret_cast<QDateTimeEdit*>(m_valueWidget);
        edit->setDateTime(value.toDateTime());
    }
    else if(m_type == CProperty::Time)
    {
        QTimeEdit* edit = reinterpret_cast<QTimeEdit*>(m_valueWidget);
        edit->setDateTime(value.toDateTime());
    }

    //emit valueChanged(this);
}

void CProperty::setEnumList(QStringList list)
{
    if(m_type == CProperty::Enum)
    {
        QComboBox* box = reinterpret_cast<QComboBox*>(m_valueWidget);
        box->clear();
        box->addItems(list);
    }
}

void CProperty::setUserData(QVariant data)
{
    m_userData = data;
}

QVariant CProperty::userData()
{
    return m_userData;
}

QToolButton* CProperty::multiFunctionButton()
{
    return m_multiFunctionButton;
}

CProperty::Type CProperty::type()
{
    return m_type;
}

QString CProperty::label()
{
    return m_label;
}

QVariant CProperty::value()
{
    if(m_type == CProperty::Hex)
    {
        QHexSpinBox* spin = reinterpret_cast<QHexSpinBox*>(m_valueWidget);
        return spin->value();
    }
    else if(m_type == CProperty::Int)
    {
        QSpinBox* spin = reinterpret_cast<QSpinBox*>(m_valueWidget);
        return spin->value();
    }
    else if (m_type == CProperty::Double)
    {
        QDoubleSpinBox* spin = reinterpret_cast<QDoubleSpinBox*>(m_valueWidget);
        return spin->value();
    }
    else if (m_type == CProperty::Bool)
    {
        QCheckBox* box = reinterpret_cast<QCheckBox*>(m_valueWidget);
        return box->isChecked();
    }
    else if(m_type == CProperty::Enum)
    {
        QComboBox* box = reinterpret_cast<QComboBox*>(m_valueWidget);
        //return box->currentText();
        return box->currentIndex();
    }
    else if(m_type == CProperty::Text)
    {
        QLineEdit* edit = reinterpret_cast<QLineEdit*>(m_valueWidget);
        return edit->text();
    }
    else if(m_type == CProperty::Pixmap)
    {
        QPushButton* button = reinterpret_cast<QPushButton*>(m_valueWidget);
        return button->text();
    }
    else if(m_type == CProperty::Label)
    {
        QLabel* label = reinterpret_cast<QLabel*>(m_valueWidget);
        label->setAlignment(Qt::AlignBottom);
        return label->text();
    }
    else if(m_type == CProperty::DateTime)
    {
        QDateTimeEdit *edit = reinterpret_cast<QDateTimeEdit*>(m_valueWidget);
        return edit->dateTime();
    }
    else if(m_type == CProperty::Time)
    {
        QTimeEdit *edit = reinterpret_cast<QTimeEdit*>(m_valueWidget);
        return edit->time();
    }

    return QVariant();
}

QWidget* CProperty::widget()
{
    return m_mainWidget;
}

pTreeItemProperty* CProperty::item()
{
    return m_item;
}

QStringList CProperty::enumList()
{

    if(m_type == CProperty::Enum)
    {
        QComboBox* box = reinterpret_cast<QComboBox*>(m_valueWidget);
        QStringList list;
        for(int i=0; i<box->count(); i++)
            list.append(box->itemText(i));

        return list;
    }

    return QStringList();
}
