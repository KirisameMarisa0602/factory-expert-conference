#include "client_globals.h"

namespace ClientGlobals {
    QString currentUser = "";
    UserRole currentUserRole = UserRole::Unknown;
    QString sessionToken = "";

    QString userRoleToString(UserRole role)
    {
        switch (role) {
        case UserRole::Factory:
            return "工厂"; // Factory
        case UserRole::Expert:
            return "专家"; // Expert
        case UserRole::Unknown:
        default:
            return "未知"; // Unknown
        }
    }

    UserRole userRoleFromString(const QString& roleStr)
    {
        if (roleStr == "工厂" || roleStr.toLower() == "factory") {
            return UserRole::Factory;
        } else if (roleStr == "专家" || roleStr.toLower() == "expert") {
            return UserRole::Expert;
        }
        return UserRole::Unknown;
    }

    QString workOrderStatusToString(WorkOrderStatus status)
    {
        switch (status) {
        case WorkOrderStatus::Created:
            return "已创建";
        case WorkOrderStatus::InProgress:
            return "进行中";
        case WorkOrderStatus::Completed:
            return "已完成";
        case WorkOrderStatus::Cancelled:
            return "已取消";
        default:
            return "未知状态";
        }
    }

    WorkOrderStatus workOrderStatusFromString(const QString& statusStr)
    {
        if (statusStr == "已创建" || statusStr.toLower() == "created") {
            return WorkOrderStatus::Created;
        } else if (statusStr == "进行中" || statusStr.toLower() == "inprogress") {
            return WorkOrderStatus::InProgress;
        } else if (statusStr == "已完成" || statusStr.toLower() == "completed") {
            return WorkOrderStatus::Completed;
        } else if (statusStr == "已取消" || statusStr.toLower() == "cancelled") {
            return WorkOrderStatus::Cancelled;
        }
        return WorkOrderStatus::Created;
    }
}